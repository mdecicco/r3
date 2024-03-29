#include <r3/common/Engine.h>
#include <r3/utils/String.h>
#include <r3/utils/Database.hpp>
#include <r3/utils/Singleton.hpp>
#include <r3/utils/Allocator.hpp>
#include <r3/utils/List.hpp>
#include <r3/utils/Application.h>
#include <r3/models/Models.hpp>
#include <r3/models/SceneModel.h>
#include <r3/models/EntityModel.h>
#include <r3/models/SceneStorage.hpp>
#include <r3/ecs/Entity.hpp>

#include <gjs/gjs.h>
#include <gjs/gjs.hpp>

#include <filesystem>

namespace r3 {
    class gjs_r3_allocator : public gjs::vm_allocator {
        public:
            virtual void* allocate(u64 size) { return Mem::alloc(size); }
            virtual void* reallocate(void* ptr, u64 size) { return Mem::realloc(ptr, size); }
            virtual void deallocate(void* ptr) { Mem::free(ptr); }
    };

    sEngine::sEngine() {
        m_args = nullptr;
        m_initialized = false;
        m_doShutdown = false;
        m_scriptBackend = nullptr;
        m_scriptCtx = nullptr;
        m_db = nullptr;
        m_app = nullptr;

        // max page size of 16384 chars, with 1024 max live allocations per page
        String::Allocator::Create(16384, 1024);
        Mem::Create();

        m_defaultLogger = new DefaultLogger();
        setLog(m_defaultLogger);
    }

    sEngine::sEngine(IApplication* app) : sEngine() {
        m_app = app;
    }

    sEngine::~sEngine() {
        if (m_app) delete m_app;

        gjs::vm_allocator* vmAlloc = nullptr;
        if (m_scriptCtx) {
            if (m_cfg.script_debug_mode) vmAlloc = ((gjs::vm_backend*)m_scriptCtx->generator())->allocator();
            delete m_scriptCtx;
        }

        if (m_scriptBackend) delete m_scriptBackend;
        if (vmAlloc) delete vmAlloc;

        if (m_args) delete m_args;
        m_args = nullptr;

        if (m_db) delete m_db;
        m_db = nullptr;

        ecs::EntityRegistry::Destroy();
        model::destroyModels();

        if (m_defaultLogger) delete m_defaultLogger;
        m_defaultLogger = nullptr;

        Mem::Destroy();
        String::Allocator::Destroy();
    }

    void sEngine::init() {
        if (m_initialized) return;

        static char cwd[512] = { 0 };
        const char* ccwd = const_cast<const char*>(cwd);

        auto p = std::filesystem::current_path() / "dummy";
        std::string path = p.string();
        for (u16 i = 0;i < path.length() && i < 512;i++) cwd[i] = path[i];

        m_cfg.process_argc = 1;
        m_cfg.process_argv = &ccwd;
        
        initializeInternal();
    }

    void sEngine::init(const engine_config& config) {
        if (m_initialized) return;

        Mem::copy(&m_cfg, &config, sizeof(engine_config));

        initializeInternal();
    }

    void sEngine::init(const Arguments& args) {
        if (m_initialized) return;
        m_args = new Arguments(args);
        m_cfg.process_argc = m_args->getRawArgCount();
        m_cfg.process_argv = m_args->getRawArgs();

        if (m_args->exists("sdbg")) m_cfg.script_debug_mode = true;
        if (m_args->exists("dir")) m_cfg.process_working_dir = m_args->getArg("dir").c_str();
        if (m_args->exists("db")) m_cfg.database_path = m_args->getArg("db").c_str();

        initializeInternal();
    }

    void sEngine::initializeInternal() {
        ecs::EntityRegistry::Create();

        if (!model::initializeModels()) {
            m_doShutdown = true;
            return;
        }

        if (m_app) {
            addUpdateSubscriber(m_app);
            m_app->onBoot(*m_args);
        }

        m_initialized = true;

        if (m_cfg.script_debug_mode) {
            m_scriptBackend = new gjs::vm_backend(new gjs_r3_allocator(), m_cfg.debug_script_stack_size, m_cfg.debug_script_mem_size);
        }
        else m_scriptBackend = new gjs::x86_backend();

        m_scriptCtx = new gjs::script_context(m_cfg.process_argc, m_cfg.process_argv, m_scriptBackend);

        if (m_cfg.process_working_dir) m_scriptCtx->io()->set_cwd(m_cfg.process_working_dir);
        else m_scriptCtx->io()->set_cwd_from_args(m_cfg.process_argc, m_cfg.process_argv);

        m_scriptCtx->compiler()->add_ir_step(gjs::optimize::ir_phase_1, false);
        m_scriptCtx->compiler()->add_ir_step(gjs::optimize::dead_code, false);

        m_db = new db::Database(m_cfg.database_path);

        if (!m_db->isValid()) {
            delete m_db;
            m_doShutdown = true;
            return;
        }

        if (m_app) m_app->onScriptBind(m_scriptCtx);

        m_scriptCtx->generator()->commit_bindings();

        model::initializeDatabase(m_db);

        i32 entryScene = m_db->count(model::SceneModel::Get(), "is_entry_point = 1");
        if (entryScene < 0) {
            m_doShutdown = true;
            return;
        }

        if (entryScene) model::initializeSceneStorage(0);

        setUpdateFrequency(60.0f);
        enableUpdates();

        i32 sessionCount = m_db->count(model::SessionModel::Get());
        if (sessionCount < 0) {
            m_doShutdown = true;
            return;
        }

        m_currentSession.id = 0;
        m_currentSession.startedOn = Datetime();
        m_currentSession.endedOn = Datetime(nullptr);
        if (!m_db->insert(model::SessionModel::Get(), m_currentSession, true)) {
            m_doShutdown = true;
            return;
        }
        
        if (m_app) {
            if (!sessionCount) m_app->onCreate(m_db);
            m_app->afterInitialize();
            model::persistSceneStorage();
        }

        if (!entryScene && !model::initializeSceneStorage(0)) {
            m_doShutdown = true;
            return;
        }
    }

    const engine_config* sEngine::getCfg() const {
        return &m_cfg;
    }

    model::SessionModel::Instance sEngine::getSessionInfo() const {
        return m_currentSession;
    }

    Arguments* sEngine::getArgs() {
        return m_args;
    }

    db::Database* sEngine::getDb() {
        return m_db;
    }

    void sEngine::addUpdateSubscriber(IPeriodicUpdate* subscriber) {
        addChild(subscriber);
    }

    void sEngine::removeUpdateSubscriber(IPeriodicUpdate* subscriber) {
        removeChild(subscriber);
    }

    void sEngine::requestShutdown() {
        m_doShutdown = true;
    }

    bool sEngine::shouldClose() const {
        return m_doShutdown;
    }

    i32 sEngine::execute() {
        if (m_doShutdown) return -1;

        m_doShutdown = getChildCount() == 0;
        if (m_doShutdown) return -2;

        model::populateSceneStorage();

        if (m_app) m_app->onSceneLoaded(model::EntityModel::Storage()->getScene());

        Timer frameTimer;
        f32 dt = 1.0f / 60.0f;
        while (!m_doShutdown) {
            if (frameTimer.stopped()) frameTimer.start();
            else {
                dt = frameTimer;
                frameTimer.reset();
                frameTimer.start();
            }

            maybeUpdate(dt);
        }

        if (m_app) m_app->beforeShutdown();

        model::persistSceneStorage();

        m_currentSession.endedOn = Datetime();
        if (!m_db->update(model::SessionModel::Get(), m_currentSession)) {
            return -3;
        }

        return 0;
    }

    void sEngine::onUpdate(f32 frameDt, f32 updateDt) {
    }
};
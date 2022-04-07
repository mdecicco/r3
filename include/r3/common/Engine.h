#pragma once
#include <r3/common/Types.h>
#include <r3/common/Logging.h>

#include <r3/utils/Arguments.h>
#include <r3/utils/Singleton.h>
#include <r3/utils/PeriodicUpdate.h>

#include <r3/models/SessionModel.h>

namespace gjs {
    class script_context;
    class backend;
};

namespace r3 {
    namespace db {
        class Database;
    };

    class IApplication;

    class sEngine : public ILoggerForwarding, private IPeriodicUpdate {
        public:
            void init();
            void init(const engine_config& config);
            void init(const Arguments& args);

            /* Accessors */
            const engine_config* getCfg() const;
            model::RawSession getSessionInfo() const;
            Arguments* getArgs();
            db::Database* getDb();

            /* Description TBD */
            void addUpdateSubscriber(IPeriodicUpdate* subscriber);
            void removeUpdateSubscriber(IPeriodicUpdate* subscriber);

            /* Runtime */
            void requestShutdown();
            bool shouldClose() const;
            i32 execute();

        protected:
            virtual void onUpdate(f32 frameDt, f32 updateDt);

        private:
            friend class Singleton<sEngine>;
            sEngine();
            sEngine(IApplication* app);
            ~sEngine();
            void initializeInternal();

            engine_config m_cfg;
            Arguments* m_args;
            bool m_doShutdown;
            bool m_initialized;
            ILogger* m_defaultLogger;
            db::Database* m_db;
            gjs::script_context* m_scriptCtx;
            gjs::backend* m_scriptBackend;
            IApplication* m_app;

            model::RawSession m_currentSession;
    };

    typedef Singleton<sEngine> Engine;
};
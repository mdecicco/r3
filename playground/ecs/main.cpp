#include <r3/r3.h>
#include <r3/models/Models.h>
#include <r3/models/SceneModel.h>
#include <r3/models/EntityModel.h>
#include <r3/models/NameComponentModel.h>
#include <r3/models/TransformComponentModel.h>
#include <r3/models/SceneStorage.hpp>
#include <r3/ecs/System.hpp>
#include <r3/ecs/Entity.h>
#include <r3/utils/Application.h>

using namespace r3;

class TestSys : public ecs::ISystem<model::RawNameComponent> {
    public:
        TestSys() {
            setUpdateInterval(0.0f);
            enableUpdates();
        }
        virtual ~TestSys() { }

        virtual void updateComponents(f32 frameDt, f32 updateDt, Array<ecs::aNameComponent>& data) {
            data.each([](ecs::aNameComponent& nc) {
                printf("Entity(%d): '%s'\n", nc->entityId, nc->name.c_str());
                nc->name = String::Format("Entity %d_%lu", nc->entityId, (u32)rand());
                nc.markChanged();
            });

            Engine::Get()->requestShutdown();
        }
};

class App : public IApplication {
    public:
        App() {
            m_test = new TestSys();
        }

        virtual ~App() {
            delete m_test;
        }

        virtual void onBoot(const Arguments& args) {
            Engine::Get()->setLogsSuppressed(true, LogCode::db_query);
            Engine::Get()->setLogsSuppressed(true, LogCode::db_begin_transaction);
            Engine::Get()->setLogsSuppressed(true, LogCode::db_commit_transaction);
        }

        virtual void afterInitialize() {
            Engine::Get()->addUpdateSubscriber(m_test);
        }

        virtual void onSceneLoaded(const model::RawScene& scene) {
            auto e = ecs::createEntity();
            e.addNameComponent();
            e.getName()->name = String::Format("Entity %d", e.getId());
            e.getName().markChanged();
        }

        virtual void onCreate(db::Database* freshDb) {
            model::RawScene entryScene = {
                0,
                "test",
                true,
                { 1000, 1000, 1000 },
                { 10, 10, 10 }
            };
            Engine::Get()->getDb()->insert(model::SceneModel::Get(), entryScene, true);
        }

        TestSys* m_test;
};

int main(int argc, const char** argv) {
    Engine::Create(new App());
    Engine::Get()->init(Arguments(argc, argv));
    i32 result = Engine::Get()->execute();
    Engine::Destroy();
    return result;
}

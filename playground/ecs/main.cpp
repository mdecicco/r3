#include <r3/r3.h>
#include <r3/models/Models.hpp>
#include <r3/models/SceneModel.h>
#include <r3/models/EntityModel.h>
#include <r3/models/NameComponentModel.h>
#include <r3/models/TransformComponentModel.h>
#include <r3/models/SceneStorage.hpp>
#include <r3/ecs/System.hpp>
#include <r3/ecs/Entity.hpp>
#include <r3/utils/Application.h>

using namespace r3;

class TestSys : public ecs::ISingleSystem<model::mNameComponent> {
    public:
        TestSys() {
            setUpdateInterval(0.0f);
            enableUpdates();
        }
        virtual ~TestSys() { }

        virtual void process(f32 frameDt, f32 updateDt, Array<model::mNameComponent>& data) {
            data.each([this](model::mNameComponent& nc, u32 idx) {
                printf("Entity(%d): '%s'\n", nc.entityId, nc.name.c_str());
                nc.name = String::Format("Entity %d_%lu", nc.entityId, (u32)rand());
                markChanged(idx);
            });

            Engine::Get()->requestShutdown();
        }
};

class TestMultiSys : public ecs::IMultiSystem<model::mNameComponent, model::mTransformComponent> {
    public:
        TestMultiSys() {
            setUpdateInterval(0.0f);
            enableUpdates();
        }

        virtual ~TestMultiSys() {}

        virtual void process(f32 frameDt, f32 updateDt, Array<ecs::Entity>& data) {
            data.each([this](ecs::Entity& e, u32 idx) {
                printf("Entity(%d): '%s' has transform\n", e.getId(), e.getComponent<model::mNameComponent>()->name.c_str());
            });
        }
};

class App : public IApplication {
    public:
        App() {
            m_test = new TestSys();
            m_multiTest = new TestMultiSys();
        }

        virtual ~App() {
            delete m_test;
            delete m_multiTest;
        }

        virtual void onBoot(const Arguments& args) {
            Engine::Get()->setLogsSuppressed(true, LogCode::db_query);
            Engine::Get()->setLogsSuppressed(true, LogCode::db_begin_transaction);
            Engine::Get()->setLogsSuppressed(true, LogCode::db_commit_transaction);
        }

        virtual void afterInitialize() {
            Engine::Get()->addUpdateSubscriber(m_test);
            Engine::Get()->addUpdateSubscriber(m_multiTest);
        }

        virtual void onSceneLoaded(const model::SceneModel::Instance& scene) {
            auto e = ecs::EntityRegistry::CreateEntity();
            e.addComponent<model::mNameComponent>({
                0,
                String::Format("Entity %d", e.getId())
            });
        }

        virtual void onCreate(db::Database* freshDb) {
            model::SceneModel::Instance entryScene = {
                0,
                "test",
                true,
                { 1000, 1000, 1000 },
                { 10, 10, 10 }
            };
            Engine::Get()->getDb()->insert(model::SceneModel::Get(), entryScene, true);
        }

        TestSys* m_test;
        TestMultiSys* m_multiTest;
};

int main(int argc, const char** argv) {
    Engine::Create(new App());
    Engine::Get()->init(Arguments(argc, argv));
    i32 result = Engine::Get()->execute();
    Engine::Destroy();
    return result;
}

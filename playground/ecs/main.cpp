#include <r3/r3.h>
#include <r3/models/Models.h>
#include <r3/models/SceneModel.h>
#include <r3/models/EntityModel.h>
#include <r3/models/NameComponentModel.h>
#include <r3/models/TransformComponentModel.h>
#include <r3/models/SceneStorage.hpp>
#include <r3/ecs/System.hpp>
#include <r3/ecs/Entity.h>

using namespace r3;

class TestSys : public ecs::ISystem<model::RawNameComponent> {
    public:
        TestSys() {
            setUpdateInterval(0.0f);
            enableUpdates();
        }
        virtual ~TestSys() { }

        virtual void updateComponents(f32 frameDt, f32 updateDt, Array<ecs::aNameComponent>& data) {
            auto e = ecs::createEntity();
            e.addNameComponent();
            e.getName()->name = String::Format("Entity %d", e.getId());
            e.getName().markChanged();

            data.each([](ecs::aNameComponent& nc) {
                printf("Entity(%d): '%s'\n", nc->entityId, nc->name.c_str());
                nc->name = String::Format("Entity %d_%lu", nc->entityId, (u32)rand());
                nc.markChanged();
            });

            Engine::Get()->requestShutdown();
        }
};

int main(int argc, const char** argv) {
    Engine::Create();
    Engine::Get()->init(Arguments(argc, argv));

    {
        Array<model::RawScene> scenes = Engine::Get()->getDb()->select(model::SceneModel::Get());
        model::RawScene s;
        if (scenes.size() == 0) {
            s = {
                0,
                "test",
                true,
                { 1000, 1000, 1000 },
                { 10, 10, 10 }
            };
            Engine::Get()->getDb()->insert(model::SceneModel::Get(), s, true);

            model::EntityStorage::Get()->setSceneId(s.id);
            model::NameComponentStorage::Get()->setSceneId(s.id);
            model::TransformComponentStorage::Get()->setSceneId(s.id);
        } else {
            model::RawScene* entry = scenes.find([](const model::RawScene& scene) { return scene.isEntryPoint; });
            if (entry) s = *entry;
            else return -1;
        }

        TestSys* test = new TestSys();
        Engine::Get()->addUpdateSubscriber(test);
        Engine::Get()->execute();
        delete test;
    }

    Engine::Destroy();
    return 0;
}

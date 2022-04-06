#include <r3/r3.h>
#include <r3/models/SceneModel.h>
#include <catch2/catch_test_macros.hpp>

using namespace r3;
TEST_CASE("Entity Systems", "[engine]") {
    Engine::Create();
    Engine::Get()->init();

    SECTION("Only one entry scene allowed") {
        Array<model::Scene> scenes = Engine::Get()->getDb()->select(model::SceneModel::Get());
        model::Scene entry = {
            0,
            "entry",
            true,
            { 1000, 1000, 1000 },
            { 10, 10, 10 }
        };

        bool entryExists = scenes.some([](const model::Scene& s) { return s.isEntryPoint; });

        if (!entryExists) {
            Engine::Get()->getDb()->insert(model::SceneModel::Get(), entry);
        }

        bool result = Engine::Get()->getDb()->insert(model::SceneModel::Get(), entry);
        REQUIRE(result == false);
    }

    i32 result = Engine::Get()->execute();
    Engine::Destroy();
}

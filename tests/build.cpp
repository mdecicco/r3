#include <r3/r3.h>
#include <catch2/catch_test_macros.hpp>

using namespace r3;
TEST_CASE("Engine runs without exception", "[engine]") {
    Engine::Create();
    Engine::Get()->init();

    i32 result = Engine::Get()->execute();
    REQUIRE(result == 0);

    Engine::Destroy();
}

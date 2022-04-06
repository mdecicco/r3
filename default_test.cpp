#include <r3/r3.h>
#include <catch2/catch_test_macros.hpp>

using namespace r3;
TEST_CASE("Insert test case here", "[test namespace here]") {
    Engine::Create();
    Engine::Get()->init();

    i32 result = Engine::Get()->execute();

    Engine::Destroy();
    return result;
}

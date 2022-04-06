#include <r3/r3.h>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

using namespace r3;
TEST_CASE("Arguments", "[engine]") {
    auto p = std::filesystem::current_path() / "dummy";
    std::string path = p.string();

    const char* args[9] = {
        p.string().c_str(),
        "yo",
        "-test",
        "abc",
        "-test1",
        "123",
        "456",
        "-test2",
        "'yo what up'"
    };

    Engine::Create();
    Engine::Get()->init(Arguments(9, args));

    i32 result = Engine::Get()->execute();
    Engine::Destroy();
}

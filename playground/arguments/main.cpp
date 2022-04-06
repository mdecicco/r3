#include <r3/r3.h>

using namespace r3;
int main(int argc, const char** argv) {
    const char* args[9] = {
        argv[0],
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
    return result;
}

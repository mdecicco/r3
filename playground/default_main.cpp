#include <r3/r3.h>

using namespace r3;
int main(int argc, const char** argv) {
    Engine::Create();
    Engine::Get()->init(Arguments(argc, argv));

    i32 result = Engine::Get()->execute();
    Engine::Destroy();
    return result;
}

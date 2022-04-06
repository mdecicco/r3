#include <r3/utils/Thread.h>
#include <thread>

namespace r3 {
    Thread::Thread() {
    }

    Thread::~Thread() {
    }

    thread_id Thread::Current() {
        return std::hash<std::thread::id>{}(std::this_thread::get_id());
    }

    void Thread::Sleep(u32 ms) {
        std::this_thread::sleep_for(std::chrono::duration<u32, std::milli>(ms));
    }
};
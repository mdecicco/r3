#pragma once
#include <r3/common/Types.h>

namespace r3 {
    class Thread {
        public:
            Thread();
            ~Thread();

            static thread_id Current();
            static void Sleep(u32 ms);
    };
};
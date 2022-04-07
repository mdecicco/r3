#pragma once
#include <r3/common/Types.h>

namespace r3 {
    class Datetime {
        public:
            Datetime();
            Datetime(void* _nullptr);
            Datetime(const Datetime& o);
            ~Datetime();

        private:
            u64 m_timestamp;
    };

    static_assert(sizeof(Datetime) == sizeof(u64), "Datetime class is not equivalent to a plain u64!");
};
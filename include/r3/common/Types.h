#pragma once
#include <stdint.h>

namespace r3 {
    typedef uint64_t    u64;
    typedef int64_t     i64;
    typedef uint32_t    u32;
    typedef int32_t     i32;
    typedef uint16_t    u16;
    typedef int16_t     i16;
    typedef uint8_t     u8;
    typedef int8_t      i8;
    typedef float       f32;
    typedef double      f64;

    struct engine_config {
        /* Scripts will be compiled to VM bytecode to allow for debugging */
        bool script_debug_mode      = false;
    };
};
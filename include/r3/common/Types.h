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
    typedef u64         thread_id;

    struct engine_config {
        /* Number of arguments supplied to the process */
        i32 process_argc = 0;

        /* Array of argument supplied to the process */
        const char** process_argv = nullptr;

        /* Desired working directory (null = program dir) */
        const char* process_working_dir = nullptr;
        
        /* Database path */
        const char* database_path = "./r3.db";

        /* Scripts will be compiled to VM bytecode to allow for debugging */
        bool script_debug_mode = false;

        /* When scripts run in debug mode, this is the maximum stack size in bytes */
        u32 debug_script_stack_size = 8 * 1024;

        /* When scripts run in debug mode, this is the maximum amount of memory they can use in bytes */
        u32 debug_script_mem_size = 32 * 1024;
    };
};
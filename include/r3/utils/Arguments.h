#pragma once
#include <r3/common/Types.h>

namespace r3 {
    class String;

    class Arguments {
        public:
            Arguments(u32 argc, char** argv);
            ~Arguments();

            void set(const String& name);
            void set(const String& name, const String& value);
            void remove(const String& name);
            bool exists(const String& name) const;
            String getArg(const String& name) const;
    };
};
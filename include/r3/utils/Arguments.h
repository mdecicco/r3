#pragma once
#include <r3/common/Types.h>
#include <r3/utils/String.h>
#include <r3/utils/robin_hood.h>

namespace r3 {
    class Arguments {
        public:
            Arguments();
            Arguments(u32 argc, const char** argv);
            ~Arguments();

            void set(const String& name);
            void set(const String& name, const String& value);
            void remove(const String& name);
            bool exists(const String& name) const;
            const String& getArg(const String& name) const;
            const String& getPath() const;

            u32 getRawArgCount() const;
            const char** getRawArgs() const;

        protected:
            robin_hood::unordered_map<String, String> m_args;
            String m_path;
            u32 m_argc;
            const char** m_argv;
    };
};
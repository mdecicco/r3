#pragma once
#include <r3/common/Types.h>
#include <r3/utils/Arguments.h>
#include <r3/common/String.h>

namespace r3 {
    class Engine {
        public:
            Engine(const Arguments& args);
            ~Engine();

            /* Accessors */
            const engine_config* getCfg() const;
            Arguments* getArgs();

            /* Runtime */
            bool shouldClose() const;
            void execute();

        protected:
            engine_config m_cfg;
            Arguments m_args;
            bool m_doShutdown;
    };
};
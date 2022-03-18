#pragma once
#include <r3/common/Types.h>

namespace r3 {
    class Engine;

    class IEngineDependant {
        public:
            IEngineDependant(Engine* e);
            ~IEngineDependant();

            Engine* getEngine() const;

        protected:
            Engine* m_engine;
    };
};
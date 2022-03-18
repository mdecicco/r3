#include <r3/common/IEngineDependant.h>

namespace r3 {
    IEngineDependant::IEngineDependant(Engine* e) : m_engine(e) {
    }

    IEngineDependant::~IEngineDependant() {
    }

    Engine* IEngineDependant::getEngine() const {
        return m_engine;
    }
};
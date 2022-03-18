#include <r3/r3.h>
#include <r3/utils/Arguments.h>

namespace r3 {
    Engine::Engine(const Arguments& args) : m_args(args) {
        m_doShutdown = false;
        
        if (m_args.exists("sdbg")) m_cfg.script_debug_mode = true;
    }

    Engine::~Engine() {
    }

    const engine_config* Engine::getCfg() const {
        return &m_cfg;
    }

    Arguments* Engine::getArgs() {
        return &m_args;
    }

    bool Engine::shouldClose() const {
        return m_doShutdown;
    }

    i32 Engine::execute() {
        while (!m_doShutdown) {
            m_doShutdown = true;
        }

        return 0;
    }
};
#include <r3/utils/Arguments.h>
#include <r3/common/String.h>

namespace r3 {
    Arguments::Arguments(u32 argc, char** argv) {
    }

    Arguments::~Arguments() {
    }

    void Arguments::set(const String& name) {
        return;
    }

    void Arguments::set(const String& name, const String& value) {
        return;
    }

    void Arguments::remove(const String& name) {
        return;
    }

    bool Arguments::exists(const String& name) const {
        return false;
    }

    String Arguments::getArg(const String& name) const {
        return String();
    }
};
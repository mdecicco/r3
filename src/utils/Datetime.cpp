#include <r3/utils/Datetime.h>
#include <time.h>

namespace r3 {
    Datetime::Datetime() : m_timestamp(time(NULL)) {
    }

    Datetime::Datetime(void* _nullptr) : m_timestamp(0) {
    }

    Datetime::Datetime(const Datetime& o) : m_timestamp(o.m_timestamp) {
    }

    Datetime::~Datetime() {
    }
};
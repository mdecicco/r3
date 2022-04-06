#include <r3/common/Logging.h>
#include <r3/utils/String.h>
#include <r3/utils/Array.hpp>
#include <stdarg.h>
#include <stdio.h>

namespace r3 {
    const char* codeFormats[] = {
        "", // error_code_start
        "FixedAllocator has reached the maximum number of live allocations allowed. Consider increasing the value of maxLiveAllocations passed to the constructor.",
        "Allocation of %llu B failed. (%llu B x %llu)",
        "Allocation of %llu B is larger than maximum possible allocation size of %llu B.",
        "Allocation of %llu B failed. Allocator has insufficient resources. (%llu B remains)",
        "Deallocation failed. (0x%llX)",
        "Failed to open DB <%s> with error: %s",
        "Failed to close DB <%s> with error: %s",
        "The model for table '%s' has already been finalized. Cannot register field '%s'",
        "The model for table '%s' must be finalized before initializing a database with it",
        "Foreign key %s.%s must refer to a primary key field",
        "Failed to finalize model for table '%s', the model is malformed",
        "Failed to finalize model for table '%s', the model has no fields registered",
        "Query failed with error: %s",
        "Database not connected",
        "Expected %d columns from table '%s', got %d",
        "Field '%s' does not exist on model for table '%s'",
        "Failed to begin transaction with error: %s",
        "Failed to commit transaction with error: %s",
        "Failed to rollback transaction with error: %s",
        "Number of rows selected does not match the number inserted (%d inserted, %d selected)",
        "Number of rows selected does not match the number inserted (%d inserted, more than %d returned)",
        "No unique field found on table '%s' which can be used to identify the row being updated",
        "Invalid Scene id provided (%d)",
        "No Scene id specified and entry Scene does not exist",
        "Invalid parameter for SceneStorage. Specified foreign key field '%s' does not refer to either 'tblEntity' or 'tblScene'. It refers to '%s'",
        "", // error_code_end

        "", // warning_code_start
        "", // warning_code_end

        "", // info_code_start
        "Executable Path: '%s'",
        "Arg[%s]: '%s'",
        "Query:\n%s",
        "Begin Transaction (level %d)",
        "Commit Transaction (level %d)",
        "Rollback Transaction (level %d)",
        "Statement '%s' was delayed by %0.3f seconds"
        ""  // info_code_end
    };



    //
    // ILogger
    //

    ILogger::ILogger() : m_isSuppressed(false) { }
    ILogger::~ILogger() { }

    void ILogger::log(LogCode code, ...) {
        if (m_isSuppressed) return;

        char msg[1024] = { 0 };
        va_list l;
        va_start(l, code);
        i32 len = vsnprintf(msg, 1024, codeFormats[(size_t)code], l);
        va_end(l);

        onMessageLogged(code, msg);
    }

    void ILogger::setLogsSuppressed(bool doSuppress) {
        m_isSuppressed = doSuppress;
    }

    bool ILogger::isLoggingSuppressed() const {
        return m_isSuppressed;
    }



    //
    // ILoggerForwarding
    //

    ILoggerForwarding::ILoggerForwarding() : m_logger(nullptr) {
    }

    ILoggerForwarding::~ILoggerForwarding() {
    }

    void ILoggerForwarding::onMessageLogged(LogCode code, const String& msg) {
        if (!m_logger || m_isSuppressed || m_logger->m_isSuppressed) return;
        m_logger->onMessageLogged(code, msg);
    }

    void ILoggerForwarding::onLoggerChanged(ILogger* prev) {
    }

    void ILoggerForwarding::setLog(ILogger* logger) {
        ILogger* prev = m_logger;
        m_logger = logger;
        onLoggerChanged(prev);
    }

    ILogger* ILoggerForwarding::getLog() const {
        return m_logger;
    }



    //
    // DefaultLogger
    //

    void DefaultLogger::onMessageLogged(LogCode code, const String& msg) {
        const char* tag = "INFO";
        if (code < LogCode::error_code_end) tag = "ERR ";
        else if (code < LogCode::warning_code_end) tag = "WARN";

        u32 idx = 0;
        msg.split({ "\n", "\r" }).each([tag, &idx](const String& s) {
            if (s.size() == 0) return;
            if (idx == 0) printf("[%s] | %s\n", tag, s.c_str());
            else printf("       | %s\n", s.c_str());
            idx++;
        });
        printf("       |\n");
    }
};
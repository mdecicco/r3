#pragma once
#include <r3/common/Types.h>

namespace r3 {
    class String;

    enum class LogCode {
        // begin error codes
        error_code_start = 0,
        max_allocations_exceeded,
        allocation_failed,
        allocation_too_large,
        insufficient_allocator_memory,
        deallocation_failed,
        db_open_failed,
        db_close_failed,
        db_model_finalized_cannot_register_field,
        db_model_not_finalized_cannot_initialize,
        db_foreign_key_must_refer_to_primary_key,
        db_finalize_model_failed_malformed,
        db_finalize_model_failed_no_fields,
        db_query_failed,
        db_not_connected,
        db_query_column_count_mismatch,
        db_query_column_not_found,
        db_begin_transaction_failed,
        db_commit_transaction_failed,
        db_rollback_transaction_failed,
        db_insert_selected_row_count_mismatch_known,
        db_insert_selected_row_count_mismatch,
        db_update_no_unique_field,
        ecs_invalid_scene_id,
        ecs_no_entry_scene,
        ecs_scene_storage_invalid_foreign_key,
        error_code_end,

        // begin warning codes
        warning_code_start,
        warning_code_end,

        // begin info codes
        info_code_start,
        program_path,
        program_argument,
        db_query,
        db_begin_transaction,
        db_commit_transaction,
        db_rollback_transaction,
        db_statement_delayed,
        info_code_end
    };

    class ILogger {
        public:
            ILogger();
            virtual ~ILogger();

            void log(LogCode code, ...);
            void setLogsSuppressed(bool doSuppress);
            bool isLoggingSuppressed() const;

        protected:
            friend class ILoggerForwarding;
            virtual void onMessageLogged(LogCode code, const String& msg) = 0;

            bool m_isSuppressed;
    };

    class ILoggerForwarding : public ILogger {
        public:
            ILoggerForwarding();
            ~ILoggerForwarding();

            void setLog(ILogger* logger);
            ILogger* getLog() const;

        protected:
            virtual void onMessageLogged(LogCode code, const String& msg);
            virtual void onLoggerChanged(ILogger* prev);

            ILogger* m_logger;
    };

    class DefaultLogger : public ILogger {
        protected:
            virtual void onMessageLogged(LogCode code, const String& msg);
    };
};
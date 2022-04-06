#include <r3/utils/Database.h>
#include <r3/utils/Thread.h>
#include <r3/utils/Timer.h>
#include <r3/utils/Array.hpp>
#include <r3/common/Engine.h>
#include <sqlite3.h>

namespace r3 {
    namespace db {
        //
        // IField
        //
    
        IField::IField(const char* tableName, const char* name, FieldType type, bool nullable, bool unique, const char* checkCond, u64 offset)
            : m_tableName(tableName), m_name(name), m_type(type), m_nullable(nullable),
              m_unique(unique), m_checkCondition(checkCond), m_offset(offset)
        {
        }

        IField::~IField() {
        }

        const char* IField::getName() const {
            return m_name;
        }

        const char* IField::getTableName() const {
            return m_tableName;
        }
        const char* IField::getCheckCondition() const {
            return m_checkCondition;
        }

        bool IField::isNullable() const{
            return m_nullable;
        }

        bool IField::isUnique() const {
            return m_unique;
        }

        FieldType IField::getType() const {
            return m_type;
        }

        u64 IField::getOffset() const {
            return m_offset;
        }



        //
        // *Field
        //

        PrimaryKeyField::PrimaryKeyField(const char* tableName, const char* name, u64 offset)
        : IField(tableName, name, FieldType::PrimaryKey, false, false, nullptr, offset) { }

        String PrimaryKeyField::genCreateStr() const {
            return "INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL";
        }

        ForeignKeyField::ForeignKeyField(const char* tableName, const char* name, PrimaryKeyField* references, bool nullable, bool unique, const char* checkCond, u64 offset)
        : IField(tableName, name, FieldType::ForeignKey, nullable, unique, checkCond, offset), m_ref(references) { }

        PrimaryKeyField* ForeignKeyField::getReference() const {
            return m_ref;
        }

        String ForeignKeyField::genCreateStr() const {
            return String::Format("INTEGER REFERENCES %s (%s)", m_ref->getTableName(), m_ref->getName());
        }

        IntegerField::IntegerField(const char* tableName, const char* name, bool nullable, bool unique, const char* checkCond, u64 offset)
        : IField(tableName, name, FieldType::Integer, nullable, unique, checkCond, offset) {
        }

        String IntegerField::genCreateStr() const {
            return "INTEGER";
        }

        BigIntegerField::BigIntegerField(const char* tableName, const char* name, bool nullable, bool unique, const char* checkCond, u64 offset)
        : IField(tableName, name, FieldType::BigInteger, nullable, unique, checkCond, offset) {
        }

        String BigIntegerField::genCreateStr() const {
            return "BIGINT";
        }

        FloatField::FloatField(const char* tableName, const char* name, bool nullable, bool unique, const char* checkCond, u64 offset)
        : IField(tableName, name, FieldType::Float, nullable, unique, checkCond, offset) {
        }

        String FloatField::genCreateStr() const {
            return "REAL";
        }

        StringField::StringField(const char* tableName, const char* name, u32 maxLen, bool nullable, bool unique, const char* checkCond, u64 offset)
        : IField(tableName, name, FieldType::String, nullable, unique, checkCond, offset), m_maxLen(maxLen) {
        }

        u32 StringField::getMaxLen() const {
            return m_maxLen;
        }

        String StringField::genCreateStr() const {
            return String::Format("VARCHAR(%d)", m_maxLen);
        }

        DatetimeField::DatetimeField(const char* tableName, const char* name, bool nullable, bool unique, const char* checkCond, u64 offset)
        : IField(tableName, name, FieldType::Datetime, nullable, unique, checkCond, offset) {
        }

        String DatetimeField::genCreateStr() const {
            return "DATETIME";
        }

        BooleanField::BooleanField(const char* tableName, const char* name, bool nullable, bool unique, const char* checkCond, u64 offset)
        : IField(tableName, name, FieldType::Boolean, nullable, unique, checkCond, offset) {
        }

        String BooleanField::genCreateStr() const {
            return "BOOLEAN";
        }

        BlobField::BlobField(const char* tableName, const char* name, u32 size, bool nullable, bool unique, const char* checkCond, u64 offset)
        : IField(tableName, name, FieldType::Blob, nullable, unique, checkCond, offset), m_size(size) {
        }

        u32 BlobField::getSize() const {
            return m_size;
        }

        String BlobField::genCreateStr() const {
            return String::Format("BLOB(%d)", m_size);
        }



        //
        // ModelBase
        //

        ModelBase::ModelBase(const char* tableName) : m_tableName(tableName), m_isFinal(false), m_isMalformed(false), m_primaryKey(nullptr) {
        }

        ModelBase::~ModelBase() {
        }

        const char* ModelBase::getTableName() const {
            return m_tableName;
        }

        const Array<IField*>& ModelBase::getFields() const {
            return m_fields;
        }

        const IField* ModelBase::getField(const char* name) const {
            return m_fields.find([name](const IField* f) {
                return strcmp(f->getName(), name) == 0;
            });
        }

        bool ModelBase::hasPrimaryKey() const {
            return m_primaryKey != nullptr;
        }

        const PrimaryKeyField* ModelBase::getPrimaryKey() const {
            return m_primaryKey;
        }

        PrimaryKeyField* ModelBase::PrimaryKeyOffset(const char* name, u64 offset) {
            if (m_isFinal) {
                Engine::Get()->log(LogCode::db_model_finalized_cannot_register_field, m_tableName, name);
                m_isMalformed = true;
                return nullptr;
            }

            m_primaryKey = new PrimaryKeyField(m_tableName, name, offset);
            m_fields.push(m_primaryKey);
            return m_primaryKey;
        }

        ForeignKeyField* ModelBase::ForeignKeyOffset(const char* name, const IField* references, u64 offset, bool nullable, bool unique, const char* checkCond) {
            if (m_isFinal) {
                Engine::Get()->log(LogCode::db_model_finalized_cannot_register_field, m_tableName, name);
                m_isMalformed = true;
                return nullptr;
            }

            if (!references || (references && references->getType() != FieldType::PrimaryKey)) {
                Engine::Get()->log(LogCode::db_foreign_key_must_refer_to_primary_key, m_tableName, name);
                m_isMalformed = true;
                return nullptr;
            }

            m_fields.push(new ForeignKeyField(m_tableName, name, (PrimaryKeyField*)references, nullable, unique, checkCond, offset));
            return (ForeignKeyField*)m_fields[m_fields.size() - 1];
        }

        IntegerField* ModelBase::IntegerOffset(const char* name, u64 offset, bool nullable, bool unique, const char* checkCond) {
            if (m_isFinal) {
                Engine::Get()->log(LogCode::db_model_finalized_cannot_register_field, m_tableName, name);
                m_isMalformed = true;
                return nullptr;
            }

            m_fields.push(new IntegerField(m_tableName, name, nullable, unique, checkCond, offset));
            return (IntegerField*)m_fields[m_fields.size() - 1];
        }

        BigIntegerField* ModelBase::BigIntegerOffset(const char* name, u64 offset, bool nullable, bool unique, const char* checkCond) {
            if (m_isFinal) {
                Engine::Get()->log(LogCode::db_model_finalized_cannot_register_field, m_tableName, name);
                m_isMalformed = true;
                return nullptr;
            }

            m_fields.push(new BigIntegerField(m_tableName, name, nullable, unique, checkCond, offset));
            return (BigIntegerField*)m_fields[m_fields.size() - 1];
        }

        FloatField* ModelBase::FloatOffset(const char* name, u64 offset, bool nullable, bool unique, const char* checkCond) {
            if (m_isFinal) {
                Engine::Get()->log(LogCode::db_model_finalized_cannot_register_field, m_tableName, name);
                m_isMalformed = true;
                return nullptr;
            }

            m_fields.push(new FloatField(m_tableName, name, nullable, unique, checkCond, offset));
            return (FloatField*)m_fields[m_fields.size() - 1];
        }

        StringField* ModelBase::StringOffset(const char* name, u32 maxLen, u64 offset, bool nullable, bool unique, const char* checkCond) {
            if (m_isFinal) {
                Engine::Get()->log(LogCode::db_model_finalized_cannot_register_field, m_tableName, name);
                m_isMalformed = true;
                return nullptr;
            }

            m_fields.push(new StringField(m_tableName, name, maxLen, nullable, unique, checkCond, offset));
            return (StringField*)m_fields[m_fields.size() - 1];
        }

        DatetimeField* ModelBase::DatetimeOffset(const char* name, u64 offset, bool nullable, bool unique, const char* checkCond) {
            if (m_isFinal) {
                Engine::Get()->log(LogCode::db_model_finalized_cannot_register_field, m_tableName, name);
                m_isMalformed = true;
                return nullptr;
            }

            m_fields.push(new DatetimeField(m_tableName, name, nullable, unique, checkCond, offset));
            return (DatetimeField*)m_fields[m_fields.size() - 1];
        }

        BooleanField* ModelBase::BooleanOffset(const char* name, u64 offset, bool nullable, bool unique, const char* checkCond) {
            if (m_isFinal) {
                Engine::Get()->log(LogCode::db_model_finalized_cannot_register_field, m_tableName, name);
                m_isMalformed = true;
                return nullptr;
            }

            m_fields.push(new BooleanField(m_tableName, name, nullable, unique, checkCond, offset));
            return (BooleanField*)m_fields[m_fields.size() - 1];
        }

        BlobField* ModelBase::BlobOffset(const char* name, u32 size, u64 offset, bool nullable, bool unique, const char* checkCond) {
            if (m_isFinal) {
                Engine::Get()->log(LogCode::db_model_finalized_cannot_register_field, m_tableName, name);
                m_isMalformed = true;
                return nullptr;
            }

            m_fields.push(new BlobField(m_tableName, name, size, nullable, unique, checkCond, offset));
            return (BlobField*)m_fields[m_fields.size() - 1];
        }

        void ModelBase::RunOnInit(const char* sql) {
            m_runOnInit.push(sql);
        }

        void ModelBase::finalize() {
            if (m_isFinal) return;
            if (m_isMalformed) {
                Engine::Get()->log(LogCode::db_finalize_model_failed_malformed, m_tableName);
                return;
            }

            if (m_fields.size() == 0) {
                Engine::Get()->log(LogCode::db_finalize_model_failed_no_fields, m_tableName);
                return;
            }

            m_isFinal = true;
        }

        void ModelBase::initializeDb(Database* db) {
            if (!m_isFinal) {
                Engine::Get()->log(LogCode::db_model_not_finalized_cannot_initialize, m_tableName);
                return;
            }

            r3::String createQuery = r3::String("CREATE TABLE IF NOT EXISTS ") + m_tableName + " (\n";

            m_fields.each([&createQuery](IField* f, u32 idx) {
                if (idx > 0) createQuery += ",\n";
                createQuery += r3::String("    ") + f->getName() + " " + f->genCreateStr();
                if (!f->isNullable()) createQuery += " NOT NULL";
                if (f->isUnique()) createQuery += " UNIQUE";
                if (f->getCheckCondition()) createQuery += r3::String(" CHECK (") + f->getCheckCondition() + ")";
            });

            createQuery += "\n)";

            db->begin();
            sqlite3_stmt* stmt = db->query(createQuery);
            if (!stmt) {
                db->rollback();
                return;
            }

            if (!db->step(stmt)) {
                db->rollback();
                db->finalize(stmt);
                return;
            }

            db->finalize(stmt);

            bool didError = m_runOnInit.some([db](const char* sql) {
                sqlite3_stmt* stmt = db->query(sql);
                if (!stmt) {
                    db->rollback();
                    return true;
                }

                if (!db->step(stmt)) {
                    db->rollback();
                    db->finalize(stmt);
                    return true;
                }

                if (!db->finalize(stmt)) {
                    db->rollback();
                    return true;
                }

                return false;
            });

            if (!didError) {
                db->commit();
            }
        }



        //
        // Database
        //

        Database::Database(const char* path) : m_db(nullptr), m_path(path), m_transactionNestIdx(0), m_doRollbackTransaction(false) {
            i32 status = 0;
            status = sqlite3_open_v2(path, &m_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, nullptr);
            if (status != SQLITE_OK) {
                Engine::Get()->log(LogCode::db_open_failed, path, sqlite3_errmsg(m_db));
                sqlite3_close(m_db);
                m_db = nullptr;
            }
        }

        Database::~Database() {
            if (m_db) {
                i32 status = sqlite3_close_v2(m_db);

                if (status != SQLITE_OK) {
                    Engine::Get()->log(LogCode::db_close_failed, m_path.c_str(), sqlite3_errmsg(m_db));
                }

                m_db = nullptr;
            }
        }

        sqlite3* Database::getSqlite3() {
            return m_db;
        }

        bool Database::isValid() const {
            return m_db != nullptr;
        }

        void Database::bindParamFromField(sqlite3_stmt* stmt, IField* f, const void* src, u32 index) {
            const u8* ptr = (const u8*)src;
            switch (f->getType()) {
                case FieldType::PrimaryKey: {
                    return;
                }
                case FieldType::ForeignKey: {
                    u32 v = *(const u32*)(ptr + f->getOffset());
                    if (v == 0) sqlite3_bind_null(stmt, index);
                    else sqlite3_bind_int(stmt, index, v);

                    break;
                }
                case FieldType::Integer: {
                    i32 v = *(const i32*)(ptr + f->getOffset());
                    sqlite3_bind_int(stmt, index, v);
                    break;
                }
                case FieldType::BigInteger: {
                    i64 v = *(const i64*)(ptr + f->getOffset());
                    sqlite3_bind_int64(stmt, index, v);
                    break;
                }
                case FieldType::Float: {
                    f32 v = *(const f32*)(ptr + f->getOffset());
                    sqlite3_bind_double(stmt, index, v);
                    break;
                }
                case FieldType::String: {
                    const String& v = *(const String*)(ptr + f->getOffset());
                    sqlite3_bind_text(stmt, index, v.c_str(), v.size(), SQLITE_TRANSIENT);
                    break;
                }
                case FieldType::Datetime: {
                    u64 v = *(const u64*)(ptr + f->getOffset());
                    sqlite3_bind_int64(stmt, index, v);
                    break;
                }
                case FieldType::Boolean: {
                    bool v = *(const bool*)(ptr + f->getOffset());
                    sqlite3_bind_int(stmt, index, v ? 1 : 0);
                    break;
                }
                case FieldType::Blob: {
                    const void* data = ptr + f->getOffset();
                    sqlite3_bind_blob(stmt, index, data, ((BlobField*)f)->getSize(), SQLITE_TRANSIENT);
                    break;
                }
            }
        }

        sqlite3_stmt* Database::query(const String& str) {
            Engine::Get()->log(LogCode::db_query, str.c_str());

            if (!m_db) {
                Engine::Get()->log(LogCode::db_not_connected, str.c_str());
                return nullptr;
            }

            sqlite3_stmt* stmt = nullptr;
            i32 status = sqlite3_prepare_v2(m_db, str.c_str(), -1, &stmt, 0);

            if (status != SQLITE_OK) {
                Engine::Get()->log(LogCode::db_query_failed, sqlite3_errmsg(m_db));

                if (stmt) {
                    sqlite3_finalize(stmt);
                }

                return nullptr;
            }

            return stmt;
        }

        i32 Database::step(sqlite3_stmt* stmt) {
            if (stmt) {
                i32 status = sqlite3_step(stmt);
                if (status == SQLITE_DONE || status == SQLITE_ROW) return status;
                else {
                    Engine::Get()->log(LogCode::db_query_failed, sqlite3_errmsg(m_db));
                    return status;
                }
            }

            return SQLITE_ERROR;
        }

        bool Database::finalize(sqlite3_stmt* stmt) {
            if (stmt) {
                i32 status = sqlite3_finalize(stmt);
                if (status != SQLITE_OK) {
                    return false;
                }

                return true;
            }

            return false;
        }

        bool Database::begin() {
            Engine::Get()->log(LogCode::db_begin_transaction, m_transactionNestIdx);

            if (m_transactionNestIdx) m_transactionNestIdx++;
            else {
                m_transactionNestIdx = 1;
                i32 status = sqlite3_exec(m_db, "BEGIN", nullptr, nullptr, nullptr);
                Timer t;

                if (status == SQLITE_BUSY) t.start();

                while (status == SQLITE_BUSY) {
                    Thread::Sleep(30);
                    status = sqlite3_exec(m_db, "BEGIN", nullptr, nullptr, nullptr);
                }

                if (t.elapsed() > 0.0f) {
                    Engine::Get()->log(LogCode::db_statement_delayed, "BEGIN", t.elapsed());
                }

                if (status != SQLITE_OK) {
                    Engine::Get()->log(LogCode::db_begin_transaction_failed, sqlite3_errmsg(m_db));
                    return false;
                }
            }

            return true;
        }

        bool Database::commit() {
            Engine::Get()->log(LogCode::db_commit_transaction, m_transactionNestIdx - 1);

            if (m_transactionNestIdx) m_transactionNestIdx--;
            else {
                Engine::Get()->log(LogCode::db_begin_transaction_failed, "No transaction to commit");
                return false;
            }

            if (!m_transactionNestIdx) {
                if (m_doRollbackTransaction) {
                    m_doRollbackTransaction = false;
                    Engine::Get()->log(LogCode::db_begin_transaction_failed, "A nested transaction requested rollback");
                    i32 status = sqlite3_exec(m_db, "ROLLBACK", nullptr, nullptr, nullptr);

                    Timer t;

                    if (status == SQLITE_BUSY) t.start();

                    while (status == SQLITE_BUSY) {
                        Thread::Sleep(30);
                        status = sqlite3_exec(m_db, "ROLLBACK", nullptr, nullptr, nullptr);
                    }

                    if (t.elapsed() > 0.0f) {
                        Engine::Get()->log(LogCode::db_statement_delayed, "ROLLBACK", t.elapsed());
                    }

                    if (status != SQLITE_OK) {
                        Engine::Get()->log(LogCode::db_rollback_transaction_failed, sqlite3_errmsg(m_db));
                        return false;
                    }
                } else {
                    i32 status = sqlite3_exec(m_db, "COMMIT", nullptr, nullptr, nullptr);

                    Timer t;

                    if (status == SQLITE_BUSY) t.start();

                    while (status == SQLITE_BUSY) {
                        Thread::Sleep(30);
                        status = sqlite3_exec(m_db, "COMMIT", nullptr, nullptr, nullptr);
                    }

                    if (t.elapsed() > 0.0f) {
                        Engine::Get()->log(LogCode::db_statement_delayed, "COMMIT", t.elapsed());
                    }

                    if (status != SQLITE_OK) {
                        Engine::Get()->log(LogCode::db_commit_transaction_failed, sqlite3_errmsg(m_db));
                        return false;
                    }
                }
            }

            return true;
        }

        bool Database::rollback() {
            Engine::Get()->log(LogCode::db_rollback_transaction, m_transactionNestIdx - 1);

            if (m_transactionNestIdx) {
                m_transactionNestIdx--;
                m_doRollbackTransaction = true;
            } else {
                Engine::Get()->log(LogCode::db_rollback_transaction_failed, "No transaction to rollback");
                return false;
            }

            if (!m_transactionNestIdx) {
                m_doRollbackTransaction = false;
                i32 status = sqlite3_exec(m_db, "ROLLBACK", nullptr, nullptr, nullptr);

                Timer t;

                if (status == SQLITE_BUSY) t.start();

                while (status == SQLITE_BUSY) {
                    Thread::Sleep(30);
                    status = sqlite3_exec(m_db, "ROLLBACK", nullptr, nullptr, nullptr);
                }

                if (t.elapsed() > 0.0f) {
                    Engine::Get()->log(LogCode::db_statement_delayed, "ROLLBACK", t.elapsed());
                }

                if (status != SQLITE_OK) {
                    Engine::Get()->log(LogCode::db_rollback_transaction_failed, sqlite3_errmsg(m_db));
                    return false;
                }
            }

            return true;
        }
    };
};
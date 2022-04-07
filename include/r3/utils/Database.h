#pragma once
#include <r3/common/Types.h>
#include <r3/utils/String.h>

struct sqlite3;
struct sqlite3_stmt;

namespace r3 {
    class Datetime;

    namespace db {
        class Database;

        enum class FieldType {
            PrimaryKey,
            ForeignKey,
            Integer,
            BigInteger,
            Float,
            String,
            Datetime,
            Boolean,
            Blob
        };


        class IField {
            public:
                IField(const char* tableName, const char* name, FieldType t, bool nullable, bool unique, const char* checkCond, u64 offset);
                ~IField();

                const char* getName() const;
                const char* getTableName() const;
                const char* getCheckCondition() const;
                bool isNullable() const;
                bool isUnique() const;
                FieldType getType() const;
                u64 getOffset() const;

                virtual String genCreateStr() const = 0;

            protected:
                const char* m_name;
                FieldType m_type;
                u64 m_offset;
                const char* m_tableName;
                bool m_nullable;
                bool m_unique;
                const char* m_checkCondition;
        };

        class PrimaryKeyField : public IField {
            public:
                PrimaryKeyField(const char* tableName, const char* name, u64 offset);
                virtual String genCreateStr() const;
        };

        class ForeignKeyField : public IField {
            public:
                ForeignKeyField(const char* tableName, const char* name, PrimaryKeyField* references, bool nullable, bool unique, const char* checkCond, u64 offset);

                PrimaryKeyField* getReference() const;

                virtual String genCreateStr() const;

            protected:
                PrimaryKeyField* m_ref;
        };

        class IntegerField : public IField {
            public:
                IntegerField(const char* tableName, const char* name, bool nullable, bool unique, const char* checkCond, u64 offset);
                virtual String genCreateStr() const;
        };

        class BigIntegerField : public IField {
            public:
                BigIntegerField(const char* tableName, const char* name, bool nullable, bool unique, const char* checkCond, u64 offset);
                virtual String genCreateStr() const;
        };

        class FloatField : public IField {
            public:
                FloatField(const char* tableName, const char* name, bool nullable, bool unique, const char* checkCond, u64 offset);
                virtual String genCreateStr() const;
        };

        class StringField : public IField {
            public:
                StringField(const char* tableName, const char* name, u32 maxLen, bool nullable, bool unique, const char* checkCond, u64 offset);
                virtual String genCreateStr() const;

                u32 getMaxLen() const;

            protected:
                u32 m_maxLen;
        };

        class DatetimeField : public IField {
            public:
                DatetimeField(const char* tableName, const char* name, bool nullable, bool unique, const char* checkCond, u64 offset);
                virtual String genCreateStr() const;
        };

        class BooleanField : public IField {
            public:
                BooleanField(const char* tableName, const char* name, bool nullable, bool unique, const char* checkCond, u64 offset);
                virtual String genCreateStr() const;
        };

        class BlobField : public IField {
            public:
                BlobField(const char* tableName, const char* name, u32 size, bool nullable, bool unique, const char* checkCond, u64 offset);
                virtual String genCreateStr() const;

                u32 getSize() const;

            protected:
                u32 m_size;
        };

        class ModelBase {
            public:
                ModelBase(const char* tableName);
                ~ModelBase();

                const char* getTableName() const;
                const Array<IField*>& getFields() const;
                const IField* getField(const char* name) const;
                bool hasPrimaryKey() const;
                const PrimaryKeyField* getPrimaryKey() const;

                PrimaryKeyField* PrimaryKeyOffset(const char* name, u64 offset);
                ForeignKeyField* ForeignKeyOffset(const char* name, const IField* references, u64 offset, bool nullable = true, bool unique = false, const char* checkCond = nullptr);
                IntegerField* IntegerOffset(const char* name, u64 offset, bool nullable = true, bool unique = false, const char* checkCond = nullptr);
                BigIntegerField* BigIntegerOffset(const char* name, u64 offset, bool nullable = true, bool unique = false, const char* checkCond = nullptr);
                FloatField* FloatOffset(const char* name, u64 offset, bool nullable = true, bool unique = false, const char* checkCond = nullptr);
                StringField* StringOffset(const char* name, u32 maxLen, u64 offset, bool nullable = true, bool unique = false, const char* checkCond = nullptr);
                DatetimeField* DatetimeOffset(const char* name, u64 offset, bool nullable = true, bool unique = false, const char* checkCond = nullptr);
                BooleanField* BooleanOffset(const char* name, u64 offset, bool nullable = true, bool unique = false, const char* checkCond = nullptr);
                BlobField* BlobOffset(const char* name, u32 size, u64 offset, bool nullable = true, bool unique = false, const char* checkCond = nullptr);

                void RunOnInit(const char* sql);
                void finalize();
                void initializeDb(Database* db);

            protected:
                PrimaryKeyField* m_primaryKey;
                bool m_isMalformed;
                bool m_isFinal;
                const char* m_tableName;
                Array<IField*> m_fields;
                Array<const char*> m_runOnInit;
        };

        template <typename Cls>
        class Model : public ModelBase {
            public:
                Model(const char* tableName);
                ~Model();
                
                PrimaryKeyField* PrimaryKey(const char* name, u32 Cls::*member);
                ForeignKeyField* ForeignKey(const char* name, const IField* references, u32 Cls::*member, bool nullable = true, bool unique = false, const char* checkCond = nullptr);
                IntegerField* Integer(const char* name, i32 Cls::*member, bool nullable = true, bool unique = false, const char* checkCond = nullptr);
                BigIntegerField* BigInteger(const char* name, i64 Cls::*member, bool nullable = true, bool unique = false, const char* checkCond = nullptr);
                FloatField* Float(const char* name, f32 Cls::*member, bool nullable = true, bool unique = false, const char* checkCond = nullptr);
                StringField* String(const char* name, u32 maxLen, r3::String Cls::*member, bool nullable = true, bool unique = false, const char* checkCond = nullptr);
                DatetimeField* Datetime(const char* name, Datetime Cls::*member, bool nullable = true, bool unique = false, const char* checkCond = nullptr);
                BooleanField* Boolean(const char* name, bool Cls::*member, bool nullable = true, bool unique = false, const char* checkCond = nullptr);

                template <typename T>
                BlobField* Blob(const char* name, u32 size, T Cls::*member, bool nullable = true, bool unique = false, const char* checkCond = nullptr);
        };

        class Database {
            public:
                Database(const char* path);
                ~Database();

                sqlite3* getSqlite3();

                // Negative value indicates error
                template <typename Cls>
                i32 count(Model<Cls>* model, const String& whereClause = "", const Array<String>& joins = {}) const;
                
                template <typename Cls>
                std::enable_if_t<std::is_default_constructible_v<Cls>, Array<Cls>>
                select(Model<Cls>* model, const String& whereClause = "", const String& orderBy = "", u32 limit = 0, u32 offset = 0, const Array<String>& joins = {});

                template <typename Cls>
                bool insertMany(Model<Cls>* model, Array<Cls>& rows, bool doSyncProvided);

                template <typename Cls>
                bool insertMany(Model<Cls>* model, const Array<Cls>& rows);

                template <typename Cls>
                bool insert(Model<Cls>* model, Cls& row, bool doSyncProvided);

                template <typename Cls>
                bool insert(Model<Cls>* model, const Cls& row);

                template <typename Cls>
                bool update(Model<Cls>* model, const Cls& row);

                sqlite3_stmt* query(const String& str) const;
                i32 step(sqlite3_stmt* stmt) const;
                bool finalize(sqlite3_stmt* stmt) const;

                bool begin();
                bool commit();
                bool rollback();

                bool isValid() const;

            protected:
                void bindParamFromField(sqlite3_stmt* stmt, IField* field, const void* src, u32 index);

                sqlite3* m_db;
                String m_path;
                u32 m_transactionNestIdx;
                bool m_doRollbackTransaction;
        };
    };
};
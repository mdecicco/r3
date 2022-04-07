#pragma once
#include <r3/utils/Database.h>
#include <r3/utils/Array.hpp>

#include <r3/common/Engine.h>
#include <sqlite3.h>

namespace r3 {
    namespace db {
        //
        // Model
        //

        template <typename Cls>
        Model<Cls>::Model(const char* tableName) : ModelBase(tableName) {
        }

        template <typename Cls>
        Model<Cls>::~Model() {
        }

        template <typename Cls>
        PrimaryKeyField* Model<Cls>::PrimaryKey(const char* name, u32 Cls::*member) {
            return PrimaryKeyOffset(name, (u8*)&((Cls*)nullptr->*member) - (u8*)nullptr);
        }

        template <typename Cls>
        ForeignKeyField* Model<Cls>::ForeignKey(const char* name, const IField* references, u32 Cls::*member, bool nullable, bool unique, const char* checkCond) {
            return ForeignKeyOffset(name, references, (u8*)&((Cls*)nullptr->*member) - (u8*)nullptr, nullable, unique, checkCond);
        }

        template <typename Cls>
        IntegerField* Model<Cls>::Integer(const char* name, i32 Cls::*member, bool nullable, bool unique, const char* checkCond) {
            return IntegerOffset(name, (u8*)&((Cls*)nullptr->*member) - (u8*)nullptr, nullable, unique, checkCond);
        }

        template <typename Cls>
        BigIntegerField* Model<Cls>::BigInteger(const char* name, i64 Cls::*member, bool nullable, bool unique, const char* checkCond) {
            return IntegerOffset(name, (u8*)&((Cls*)nullptr->*member) - (u8*)nullptr, nullable, unique, checkCond);
        }

        template <typename Cls>
        FloatField* Model<Cls>::Float(const char* name, f32 Cls::*member, bool nullable, bool unique, const char* checkCond) {
            return FloatOffset(name, (u8*)&((Cls*)nullptr->*member) - (u8*)nullptr, nullable, unique, checkCond);
        }

        template <typename Cls>
        StringField* Model<Cls>::String(const char* name, u32 maxLen, r3::String Cls::*member, bool nullable, bool unique, const char* checkCond) {
            return StringOffset(name, maxLen, (u8*)&((Cls*)nullptr->*member) - (u8*)nullptr, nullable, unique, checkCond);
        }

        template <typename Cls>
        DatetimeField* Model<Cls>::Datetime(const char* name, r3::Datetime Cls::*member, bool nullable, bool unique, const char* checkCond) {
            return DatetimeOffset(name, (u8*)&((Cls*)nullptr->*member) - (u8*)nullptr, nullable, unique, checkCond);
        }

        template <typename Cls>
        BooleanField* Model<Cls>::Boolean(const char* name, bool Cls::*member, bool nullable, bool unique, const char* checkCond) {
            return BooleanOffset(name, (u8*)&((Cls*)nullptr->*member) - (u8*)nullptr, nullable, unique, checkCond);
        }

        template <typename Cls>
        template <typename T>
        BlobField* Model<Cls>::Blob(const char* name, u32 size, T Cls::*member, bool nullable, bool unique, const char* checkCond) {
            return BlobOffset(name, size, (u8*)&((Cls*)nullptr->*member) - (u8*)nullptr, nullable, unique, checkCond);
        }


        
        //
        // Database
        //

        template <typename Cls>
        i32 Database::count(Model<Cls>* model, const String& whereClause, const Array<String>& joins) const {
            String q = "SELECT COUNT(*) FROM ";
            q += model->getTableName();

            u32 jidx = 0;
            joins.each([&q, &jidx](const String& j) {
                if (j.size() == 0) return;
                if (jidx == 0) q += '\n';
                q += j;
                q += '\n';
                jidx++;
            });

            if (whereClause.size() > 0) {
                if (jidx == 0) q += " WHERE ";
                else q += "WHERE ";
                q += whereClause;
            }

            i32 rowCount = 0;
            sqlite3_stmt* countStmt = query(q);
            if (countStmt) {
                if (step(countStmt) == SQLITE_ROW) {
                    rowCount = sqlite3_column_int(countStmt, 0);
                } else {
                    finalize(countStmt);
                    return -1;
                }

                finalize(countStmt);
            }

            return rowCount;
        }

        template <typename Cls>
        std::enable_if_t<std::is_default_constructible_v<Cls>, Array<Cls>>
        Database::select(Model<Cls>* model, const String& whereClause, const String& orderBy, u32 limit, u32 offset, const Array<String>& joins) {
            String q = "FROM ";
            q += model->getTableName();

            u32 jidx = 0;
            joins.each([&q, &jidx](const String& j) {
                if (j.size() == 0) return;
                if (jidx == 0) q += '\n';
                q += j;
                q += '\n';
                jidx++;
            });

            if (whereClause.size() > 0) {
                if (jidx == 0) q += " WHERE ";
                else q += "WHERE ";
                q += whereClause;
            }

            if (orderBy.size() > 0) {
                q += " ORDER BY ";
                q += orderBy;
            }

            if (limit) {
                q += String::Format(" LIMIT %d", limit);
                if (offset) q += String::Format(" OFFSET %d", offset);
            }

            u32 rowCount = 0;
            sqlite3_stmt* countStmt = query(String("SELECT COUNT(*) ") + q);
            if (countStmt) {
                if (step(countStmt) == SQLITE_ROW) {
                    rowCount = sqlite3_column_int(countStmt, 0);
                    if (limit && limit < rowCount) rowCount = limit;
                } else {
                    finalize(countStmt);
                    return Array<Cls>();
                }

                finalize(countStmt);
            }

            if (rowCount == 0) return Array<Cls>();

            const Array<IField*>& fields = model->getFields();
            sqlite3_stmt* stmt = query(String("SELECT ") + model->getTableName() + ".* " + q);
            if (stmt) {
                Array<Cls> out;
                i32 status = 0;
                while ((status = step(stmt)) == SQLITE_ROW) {
                    i32 ccount = sqlite3_column_count(stmt);
                    if (ccount != fields.size()) {
                        Engine::Get()->log(LogCode::db_query_column_count_mismatch, fields.size(), model->getTableName(), ccount);
                        break;
                    }

                    Cls obj;
                    u8* ptr = (u8*)&obj;

                    bool failed = false;
                    for (u32 f = 0;f < fields.size();f++) {
                        bool found = false;
                        String cname = sqlite3_column_name(stmt, f);
                        for (u32 mf = 0;mf < fields.size();mf++) {
                            IField* field = fields[mf];
                            if (cname == field->getName()) {
                                found = true;

                                switch (field->getType()) {
                                    case FieldType::PrimaryKey:
                                    case FieldType::ForeignKey: {
                                        i32 v = sqlite3_column_int(stmt, f);
                                        *(u32*)(ptr + field->getOffset()) = v;
                                        break;
                                    }
                                    case FieldType::Integer: {
                                        *(i32*)(ptr + field->getOffset()) = sqlite3_column_int(stmt, f);
                                        break;
                                    }
                                    case FieldType::BigInteger: {
                                        *(i64*)(ptr + field->getOffset()) = sqlite3_column_int64(stmt, f);
                                        break;
                                    }
                                    case FieldType::Float: {
                                        *(f32*)(ptr + field->getOffset()) = (f32)sqlite3_column_double(stmt, f);
                                        break;
                                    }
                                    case FieldType::String: {
                                        *(String*)(ptr + field->getOffset()) = (const char*)sqlite3_column_text(stmt, f);
                                        break;
                                    }
                                    case FieldType::Datetime: {
                                        *(u64*)(ptr + field->getOffset()) = sqlite3_column_int64(stmt, f);
                                        break;
                                    }
                                    case FieldType::Boolean: {
                                        *(bool*)(ptr + field->getOffset()) = sqlite3_column_int(stmt, f);
                                        break;
                                    }
                                    case FieldType::Blob: {
                                        const void* data = sqlite3_column_blob(stmt, f);
                                        Mem::copy(ptr + field->getOffset(), data, ((BlobField*)field)->getSize());
                                        break;
                                    }
                                }

                                break;
                            }
                        }

                        if (!found) {
                            Engine::Get()->log(LogCode::db_query_column_not_found, cname.c_str(), model->getTableName());
                            failed = true;
                            break;
                        }
                    }

                    if (failed) break;
                    out.push(obj);
                }

                finalize(stmt);
                return out;
            }

            return Array<Cls>();
        }

        template <typename Cls>
        bool Database::insertMany(Model<Cls>* model, Array<Cls>& rows, bool doSyncProvided) {
            if (doSyncProvided) begin();

            bool insertResult = insertMany(model, const_cast<const Array<Cls>&>(rows));

            if (!insertResult) {
                if (doSyncProvided) rollback();
                return false;
            }

            if (!doSyncProvided) return true;

            const char* idCol = model->hasPrimaryKey() ? model->getPrimaryKey()->getName() : "ROWID";

            String q = String::Format("FROM %s ORDER BY %s DESC LIMIT %d", model->getTableName(), idCol, rows.size());

            u32 rowCount = 0;
            sqlite3_stmt* countStmt = query(String("SELECT COUNT(*) ") + q);
            if (countStmt) {
                if (step(countStmt) == SQLITE_ROW) {
                    rowCount = sqlite3_column_int(countStmt, 0);
                    if (rowCount > rows.size()) rowCount = rows.size();
                } else {
                    finalize(countStmt);
                    rollback();
                    return false;
                }

                finalize(countStmt);
            }

            if (rowCount != rows.size()) {
                Engine::Get()->log(LogCode::db_insert_selected_row_count_mismatch_known, rows.size(), rowCount);
                rollback();
                return false;
            }

            const Array<IField*>& fields = model->getFields();
            sqlite3_stmt* stmt = query(String("SELECT ") + model->getTableName() + ".* " + q);
            if (stmt) {
                i32 status = 0;

                // array must be updated from the end to the front due to ORDER BY <idCol> DESC
                u32 ridx = rows.size();
                bool failed = false;
                while ((status = step(stmt)) == SQLITE_ROW) {
                    if (ridx == 0) {
                        Engine::Get()->log(LogCode::db_insert_selected_row_count_mismatch, rows.size(), rows.size());
                        failed = true;
                        break;
                    }

                    i32 ccount = sqlite3_column_count(stmt);
                    if (ccount != fields.size()) {
                        Engine::Get()->log(LogCode::db_query_column_count_mismatch, fields.size(), model->getTableName(), ccount);
                        break;
                    }

                    u8* ptr = (u8*)&rows[--ridx];

                    for (u32 f = 0;f < fields.size();f++) {
                        bool found = false;
                        String cname = sqlite3_column_name(stmt, f);
                        for (u32 mf = 0;mf < fields.size();mf++) {
                            IField* field = fields[mf];
                            if (cname == field->getName()) {
                                found = true;

                                switch (field->getType()) {
                                    case FieldType::PrimaryKey:
                                    case FieldType::ForeignKey: {
                                        i32 v = sqlite3_column_int(stmt, f);
                                        *(u32*)(ptr + field->getOffset()) = v;
                                        break;
                                    }
                                    case FieldType::Integer: {
                                        *(i32*)(ptr + field->getOffset()) = sqlite3_column_int(stmt, f);
                                        break;
                                    }
                                    case FieldType::BigInteger: {
                                        *(i64*)(ptr + field->getOffset()) = sqlite3_column_int64(stmt, f);
                                        break;
                                    }
                                    case FieldType::Float: {
                                        *(f32*)(ptr + field->getOffset()) = (f32)sqlite3_column_double(stmt, f);
                                        break;
                                    }
                                    case FieldType::String: {
                                        *(String*)(ptr + field->getOffset()) = (const char*)sqlite3_column_text(stmt, f);
                                        break;
                                    }
                                    case FieldType::Datetime: {
                                        *(u64*)(ptr + field->getOffset()) = sqlite3_column_int64(stmt, f);
                                        break;
                                    }
                                    case FieldType::Boolean: {
                                        *(bool*)(ptr + field->getOffset()) = sqlite3_column_int(stmt, f);
                                        break;
                                    }
                                    case FieldType::Blob: {
                                        const void* data = sqlite3_column_blob(stmt, f);
                                        Mem::copy(ptr + field->getOffset(), data, ((BlobField*)field)->getSize());
                                        break;
                                    }
                                }

                                break;
                            }
                        }

                        if (!found) {
                            Engine::Get()->log(LogCode::db_query_column_not_found, cname.c_str(), model->getTableName());
                            failed = true;
                            break;
                        }
                    }

                    if (failed) break;
                }

                finalize(stmt);
                if (failed) rollback();
            } else rollback();

            commit();

            return true;
        }

        template <typename Cls>
        bool Database::insertMany(Model<Cls>* model, const Array<Cls>& rows) {
            u32 batchSize = 2000 / model->getFields().size() - (model->hasPrimaryKey() ? 1 : 0);
            u32 rowBegin = 0;
            u32 rowEnd = std::min(batchSize, rows.size());

            String qbase = String("INSERT INTO ") + model->getTableName() + " (\n";
            u32 cidx = 0;
            model->getFields().each([&qbase, &cidx](IField* f) {
                if (f->getType() == FieldType::PrimaryKey) return;

                if (cidx > 0) qbase += ",\n";
                qbase += String("    ") + f->getName();
                cidx++;
            });
            qbase += "\n)\nVALUES\n";

            begin();

            bool failed = false;
            while (rowEnd <= rows.size()) {
                String q;
                for (u32 bidx = 0, ridx = rowBegin;ridx < rowEnd;ridx++,bidx++) {
                    if (bidx > 0) q += ",\n";
                    q += "    (";

                    u32 idx = 0;
                    model->getFields().each([&q, &idx](IField* f) {
                        if (f->getType() == FieldType::PrimaryKey) return;
                        if (idx > 0) q += ", ";
                        q += "?";
                        idx++;
                    });

                    q += ")";
                }

                sqlite3_stmt* stmt = query(qbase + q);
                if (stmt) {
                    u32 pidx = 1;
                    for (u32 ridx = rowBegin;ridx < rowEnd;ridx++) {
                        const Cls* optr = &rows[ridx];
                        const u8* ptr = (u8*)optr;
                        model->getFields().each([this, &pidx, stmt, ptr](IField* f) {
                            if (f->getType() == FieldType::PrimaryKey) return;
                            bindParamFromField(stmt, f, (const void*)ptr, pidx++);
                        });
                    }
                } else {
                    failed = true;
                    break;
                }

                if (step(stmt) != SQLITE_DONE) {
                    failed = true;
                    finalize(stmt);
                    break;
                }

                if (!finalize(stmt)) {
                    failed = true;
                    break;
                }

                rowBegin += batchSize;
                rowEnd += batchSize;

                if (rowBegin < rows.size() && rowEnd > rows.size()) {
                    rowEnd = rows.size();
                }
            }

            if (failed) rollback();
            else commit();

            return !failed;
        }

        template <typename Cls>
        bool Database::insert(Model<Cls>* model, Cls& row, bool doSyncProvided) {
            Array<Cls> arr = { row };
            bool ret = insertMany<Cls>(model, arr, doSyncProvided);
            if (!ret || !doSyncProvided) return ret;

            const Array<IField*>& fields = model->getFields();
            bool failed = false;
            u8* srcPtr = (u8*)&arr[0];
            u8* ptr = (u8*)&row;

            for (u32 f = 0;f < fields.size();f++) {
                for (u32 mf = 0;mf < fields.size();mf++) {
                    IField* field = fields[mf];
                    switch (field->getType()) {
                        case FieldType::PrimaryKey:
                        case FieldType::ForeignKey: {
                            *(u32*)(ptr + field->getOffset()) = *(u32*)(srcPtr + field->getOffset());
                            break;
                        }
                        case FieldType::Integer: {
                            *(i32*)(ptr + field->getOffset()) = *(i32*)(srcPtr + field->getOffset());
                            break;
                        }
                        case FieldType::BigInteger: {
                            *(i64*)(ptr + field->getOffset()) = *(i64*)(srcPtr + field->getOffset());
                            break;
                        }
                        case FieldType::Float: {
                            *(f32*)(ptr + field->getOffset()) = *(f32*)(srcPtr + field->getOffset());
                            break;
                        }
                        case FieldType::String: {
                            *(String*)(ptr + field->getOffset()) = *(String*)(srcPtr + field->getOffset());
                            break;
                        }
                        case FieldType::Datetime: {
                            *(u64*)(ptr + field->getOffset()) = *(u64*)(srcPtr + field->getOffset());
                            break;
                        }
                        case FieldType::Boolean: {
                            *(bool*)(ptr + field->getOffset()) = *(bool*)(srcPtr + field->getOffset());
                            break;
                        }
                        case FieldType::Blob: {
                            Mem::copy(ptr + field->getOffset(), srcPtr + field->getOffset(), ((BlobField*)field)->getSize());
                            break;
                        }
                    }
                }
            }

            return true;
        }

        template <typename Cls>
        bool Database::insert(Model<Cls>* model, const Cls& row) {
            return insertMany<Cls>(model, { row });
        }

        template <typename Cls>
        bool Database::update(Model<Cls>* model, const Cls& row) {
            String q = String("UPDATE ") + model->getTableName() + "\nSET\n";
            IField* unique = (IField*)model->getPrimaryKey();
            u32 fidx = 0;
            model->getFields().each([&q, &unique, &fidx](IField* f) {
                if (f->getType() == FieldType::PrimaryKey) return;
                if (!unique && f->isUnique()) unique = f;
                if (fidx > 0) q += ",\n";
                fidx++;
                q += String::Format("    %s = ?", f->getName());
            });
            q += "\n";

            if (!unique) {
                Engine::Get()->log(LogCode::db_update_no_unique_field, model->getTableName());
                return false;
            }

            q += String::Format("WHERE %s = ?", unique->getName());
            sqlite3_stmt* stmt = query(q);
            if (!stmt) return false;

            u32 pidx = 1;
            model->getFields().each([stmt, this, &row, &pidx](IField* f) {
                if (f->getType() == FieldType::PrimaryKey) return;
                bindParamFromField(stmt, f, (const void*)&row, pidx++);
            });

            bindParamFromField(stmt, unique, (const void*)&row, pidx++);

            if (step(stmt) != SQLITE_DONE) {
                finalize(stmt);
                return false;
            }

            if (!finalize(stmt)) {
                return false;
            }

            return true;
        }
    };
};
#include <r3/models/SceneModel.h>
#include <r3/utils/Singleton.hpp>
#include <r3/utils/Database.hpp>

namespace r3 {
    namespace model {
        void SceneModel::initModel(db::Model<RawScene>* m) {
            m->RunOnInit(""
                "CREATE TRIGGER IF NOT EXISTS tblScene_insert_one_entry\n"
                "BEFORE INSERT ON tblScene FOR EACH ROW\n"
                "BEGIN\n"
                "   SELECT CASE WHEN\n"
                "       NEW.is_entry_point = 1 AND (SELECT COUNT(id) FROM tblScene WHERE is_entry_point = 1) > 0\n"
                "   THEN\n"
                "       RAISE(FAIL, \"There is already an entry scene\")\n"
                "   END;\n"
                "END;\n"
            );
            m->RunOnInit(""
                "CREATE TRIGGER IF NOT EXISTS tblScene_update_one_entry\n"
                "BEFORE UPDATE ON tblScene FOR EACH ROW\n"
                "BEGIN\n"
                "   SELECT CASE WHEN\n"
                "       NEW.is_entry_point = 1 AND (SELECT COUNT(id) FROM tblScene WHERE is_entry_point = 1) > 0\n"
                "   THEN\n"
                "       RAISE(FAIL, \"There is already an entry scene\")\n"
                "   END;\n"
                "END;\n"
            );
        }
    };
};
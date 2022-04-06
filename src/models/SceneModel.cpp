#include <r3/models/SceneModel.h>
#include <r3/utils/Singleton.hpp>
#include <r3/utils/Database.hpp>

namespace r3 {
    namespace model {
        void SceneModel::Init() {
            SceneModel::Create("tblScene");
            auto* m = SceneModel::Get();
            m->PrimaryKey("id", &RawScene::id);
            m->String("name", 64, &RawScene::name, false, true);
            m->Boolean("is_entry_point", &RawScene::isEntryPoint, false, false);
            m->IntegerOffset("width", offsetof(RawScene, dims) + offsetof(RawScene::_dims, width), false);
            m->IntegerOffset("height", offsetof(RawScene, dims) + offsetof(RawScene::_dims, height), false);
            m->IntegerOffset("length", offsetof(RawScene, dims) + offsetof(RawScene::_dims, length), false);
            m->IntegerOffset("width_divs", offsetof(RawScene, divs) + offsetof(RawScene::_divs, width), false);
            m->IntegerOffset("height_divs", offsetof(RawScene, divs) + offsetof(RawScene::_divs, height), false);
            m->IntegerOffset("length_divs", offsetof(RawScene, divs) + offsetof(RawScene::_divs, length), false);
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
            m->finalize();
        }
    };
};
#include <r3/models/EntityModel.h>
#include <r3/models/SceneModel.h>
#include <r3/models/SceneStorage.hpp>
#include <r3/utils/Singleton.hpp>
#include <r3/utils/Database.hpp>

namespace r3 {
    namespace model {
        void EntityModel::Init() {
            EntityModel::Create("tblEntity");
            auto* m = EntityModel::Get();
            auto* pk = m->PrimaryKey("id", &RawEntity::id);
            m->ForeignKey("parent_id", pk, &RawEntity::parentId, true, false, "parent_id != id");
            auto* sceneId = m->ForeignKey("scene_id", SceneModel::Get()->getPrimaryKey(), &RawEntity::sceneId);
            m->finalize();

            EntityStorage::Create(m, sceneId);
        }
    };
};
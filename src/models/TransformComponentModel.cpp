#include <r3/models/TransformComponentModel.h>
#include <r3/models/SceneStorage.hpp>
#include <r3/utils/Singleton.hpp>
#include <r3/utils/Database.hpp>

namespace r3 {
    namespace model {
        void TransformComponentModel::Init() {
            TransformComponentModel::Create("tblTransformComponent");
            auto* m = TransformComponentModel::Get();
            auto* entityId = m->ForeignKey("entity_id", EntityModel::Get()->getPrimaryKey(), &RawTransformComponent::entityId, false, true);
            m->Integer("cell_idx", &RawTransformComponent::cellIdx, false, false, "cell_idx > 0 OR cell_idx = -1");
            m->FloatOffset("pos_x", offsetof(RawTransformComponent, position) + 0, false);
            m->FloatOffset("pos_y", offsetof(RawTransformComponent, position) + 4, false);
            m->FloatOffset("pos_z", offsetof(RawTransformComponent, position) + 8, false);
            m->FloatOffset("scale_x", offsetof(RawTransformComponent, scale) + 0, false);
            m->FloatOffset("scale_y", offsetof(RawTransformComponent, scale) + 4, false);
            m->FloatOffset("scale_z", offsetof(RawTransformComponent, scale) + 8, false);
            m->FloatOffset("rot_x", offsetof(RawTransformComponent, rotation) + 0, false);
            m->FloatOffset("rot_y", offsetof(RawTransformComponent, rotation) + 4, false);
            m->FloatOffset("rot_z", offsetof(RawTransformComponent, rotation) + 8, false);
            m->FloatOffset("rot_w", offsetof(RawTransformComponent, rotation) + 12, false);
            m->finalize();

            TransformComponentStorage::Create(m, entityId);
        }
    };
};
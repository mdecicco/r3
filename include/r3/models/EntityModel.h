#pragma once
#include <r3/common/Types.h>
#include <r3/models/Models.h>
#include <r3/models/SceneModel.h>

namespace r3 {
    namespace model {
        typedef u32 EntityId;
        struct mEntity {
            EntityId id;
            EntityId parentId;
            SceneId sceneId;
        };

        class EntityModel : public ModelBase<mEntity, "tblEntity", EntityModel> {
            public:
                PrimaryKey<"id", &Instance::id> id;
                ForeignKey<"parent_id", &Instance::parentId, &EntityModel::id, true, false, "parent_id != id"> parentId;
                ForeignKey<"scene_id", &Instance::sceneId, &SceneModel::id> sceneId;
        };

        struct mComponentBase {
            EntityId entityId;
        };

        template <typename T, typename T2 = void>
        struct is_entity_component : std::false_type { };

        template <typename T>
        struct is_entity_component<T> : std::is_base_of<mComponentBase, T> { };
    };
};
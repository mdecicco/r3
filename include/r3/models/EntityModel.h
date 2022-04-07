#pragma once
#include <r3/common/Types.h>
#include <r3/models/Models.h>
#include <r3/models/SceneModel.h>

namespace r3 {
    namespace model {
        typedef u32 EntityId;
        struct RawEntity {
            EntityId id;
            EntityId parentId;
            SceneId sceneId;
        };

        class EntityModel : public ModelBase<RawEntity, "tblEntity", EntityModel> {
            public:
                PrimaryKey<"id", &RawEntity::id> id;
                ForeignKey<"parent_id", &RawEntity::parentId, &EntityModel::id, true, false, "parent_id != id"> parentId;
                ForeignKey<"scene_id", &RawEntity::sceneId, &SceneModel::id> sceneId;
        };

        struct RawComponentBase {
            EntityId entityId;
        };

        template <typename T, typename T2 = void>
        struct is_entity_component : std::false_type { };

        template <typename T>
        struct is_entity_component<T> : std::is_base_of<RawComponentBase, T> { };
    };
};
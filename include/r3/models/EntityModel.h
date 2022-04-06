#pragma once
#include <r3/common/Types.h>
#include <r3/utils/Singleton.h>
#include <r3/utils/Database.h>

#include <r3/models/SceneModel.h>
#include <r3/models/SceneStorage.h>

namespace r3 {
    namespace model {
        struct RawEntity;
        class EntityModel : public Singleton<db::Model<RawEntity>> {
            public:
                static void Init();
        };

        typedef Singleton<SceneStorage<RawEntity>> EntityStorage;

        typedef u32 EntityId;
        struct RawEntity {
            EntityId id;
            EntityId parentId;
            SceneId sceneId;
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
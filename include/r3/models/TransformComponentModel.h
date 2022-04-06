#pragma once
#include <r3/common/Types.h>

#include <r3/utils/Singleton.h>
#include <r3/utils/Database.h>
#include <r3/utils/Math.h>

#include <r3/models/EntityModel.h>
#include <r3/models/SceneStorage.h>


namespace r3 {
    namespace model {
        struct RawTransformComponent;
        class TransformComponentModel : public Singleton<db::Model<RawTransformComponent>> {
            public:
                static void Init();
        };

        typedef Singleton<SceneStorage<RawTransformComponent>> TransformComponentStorage;

        struct RawTransformComponent : RawComponentBase {
            i32 cellIdx;

            vec3 position;
            vec3 scale;
            quat rotation;
        };
    };
};
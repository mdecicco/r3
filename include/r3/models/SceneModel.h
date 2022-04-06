#pragma once
#include <r3/common/Types.h>
#include <r3/utils/Singleton.h>
#include <r3/utils/Database.h>

namespace r3 {
    namespace model {
        struct RawScene;
        class SceneModel : public Singleton<db::Model<RawScene>> {
            public:
                static void Init();
        };

        typedef u32 SceneId;
        typedef u32 SceneCellId;
        struct RawScene {
            SceneId id;
            String name;
            bool isEntryPoint;

            struct _dims {
                i32 width;
                i32 height;
                i32 length;
            } dims;

            struct _divs {
                i32 width;
                i32 height;
                i32 length;
            } divs;
        };
    };
};
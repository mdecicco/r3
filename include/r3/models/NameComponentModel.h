#pragma once
#include <r3/common/Types.h>

#include <r3/utils/Singleton.h>
#include <r3/utils/Database.h>

#include <r3/models/EntityModel.h>
#include <r3/models/SceneStorage.h>

namespace r3 {
    namespace model {
        struct RawNameComponent;
        class NameComponentModel : public Singleton<db::Model<RawNameComponent>> {
            public:
                static void Init();
        };

        typedef Singleton<SceneStorage<RawNameComponent>> NameComponentStorage;

        struct RawNameComponent : RawComponentBase {
            String name;
        };
    };
};
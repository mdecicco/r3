#pragma once
#include <r3/common/Types.h>
#include <r3/models/EntityModel.h>

namespace r3 {
    namespace model {
        struct mNameComponent : mComponentBase {
            String name;
        };

        class NameComponentModel : public ModelBase<mNameComponent, "tblNameComponent", NameComponentModel> {
            public:
                ForeignKey<"entity_id", &Instance::entityId, &EntityModel::id, false, true> entityId;
                String<"name", &Instance::name, 64> name;
        };
    };
};
#pragma once
#include <r3/common/Types.h>
#include <r3/models/EntityModel.h>

namespace r3 {
    namespace model {
        struct RawNameComponent : RawComponentBase {
            String name;
        };

        class NameComponentModel : public ModelBase<RawNameComponent, "tblNameComponent", NameComponentModel> {
            public:
                using Raw = RawNameComponent;
                using Self = NameComponentModel;

                ForeignKey<"entity_id", &Raw::entityId, &EntityModel::id, false, true> entityId;
                String<"name", &Raw::name, 64> name;
        };
    };
};
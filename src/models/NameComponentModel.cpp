#include <r3/models/NameComponentModel.h>
#include <r3/models/SceneStorage.hpp>
#include <r3/utils/Singleton.hpp>
#include <r3/utils/Database.hpp>

namespace r3 {
    namespace model {
        void NameComponentModel::Init() {
            NameComponentModel::Create("tblNameComponent");
            auto* m = NameComponentModel::Get();
            auto* entityId = m->ForeignKey("entity_id", EntityModel::Get()->getPrimaryKey(), &RawNameComponent::entityId, false, true);
            m->String("name", 64, &RawNameComponent::name, false);
            m->finalize();

            NameComponentStorage::Create(m, entityId);
        }
    };
};
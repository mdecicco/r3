#pragma once
#include <r3/common/Types.h>
#include <r3/utils/PeriodicUpdate.h>
#include <r3/models/EntityModel.h>
#include <r3/models/SceneStorage.h>

namespace r3 {
    namespace ecs {
        template <typename RawModel>
        class ISystem : public IPeriodicUpdate {
            static_assert(model::is_entity_component<RawModel>::value, "ISystem template argument must be a derivative of 'model::RawComponentBase'");
            public:
                ISystem();
                virtual ~ISystem();

                virtual void updateComponents(f32 frameDt, f32 updateDt, Array<model::ModelAccessor<RawModel>>& data) = 0;

            private:
                virtual void onUpdate(f32 frameDt, f32 updateDt);
        };
    };
};
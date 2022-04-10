#pragma once
#include <r3/common/Types.h>
#include <r3/utils/PeriodicUpdate.h>
#include <r3/models/EntityModel.h>
#include <r3/models/SceneStorage.h>
#include <r3/ecs/Entity.h>

namespace r3 {
    namespace ecs {
        template <typename...>
        inline constexpr auto _are_all_components = std::true_type{};

        template <typename T, typename... Rest>
        inline constexpr auto _are_all_components<T, Rest...> = std::bool_constant<
            model::is_entity_component<T>::value && _are_all_components<Rest...>
        >{};

        template <typename RawModel>
        class ISingleSystem : public IPeriodicUpdate {
            static_assert(model::is_entity_component<RawModel>::value, "ISingleSystem template argument must be a derivative of 'model::mComponentBase'");
            public:
                ISingleSystem();
                virtual ~ISingleSystem();

                virtual void process(f32 frameDt, f32 updateDt, Array<RawModel>& models) = 0;
                void markClean(u32 idx);
                void markChanged(u32 idx);
                void markDeleted(u32 idx);

            private:
                virtual void onUpdate(f32 frameDt, f32 updateDt) final;
        };
        
        template <typename ...RawModels>
        class IMultiSystem : public IPeriodicUpdate {
            static_assert(_are_all_components<RawModels...>, "IMultiSystem template arguments must be derivatives of 'model::mComponentBase'");
            public:
                IMultiSystem();
                virtual ~IMultiSystem();

                virtual void process(f32 frameDt, f32 updateDt, Array<Entity>& models) = 0;

            private:
                virtual void onUpdate(f32 frameDt, f32 updateDt) final;
        };
    };
};
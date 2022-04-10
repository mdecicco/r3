#pragma once
#include <r3/ecs/System.h>
#include <r3/ecs/Entity.hpp>
#include <r3/models/SceneStorage.hpp>
#include <r3/utils/Singleton.hpp>
#include <r3/utils/Array.hpp>

namespace r3 {
    namespace ecs {
        template <typename RawModel>
        ISingleSystem<RawModel>::ISingleSystem() {
        }

        template <typename RawModel>
        ISingleSystem<RawModel>::~ISingleSystem() {
        }

        template <typename RawModel>
        void ISingleSystem<RawModel>::onUpdate(f32 frameDt, f32 updateDt) {
            process(frameDt, updateDt, Singleton<model::SceneStorage<RawModel>>::Get()->getMutableRawData());
        }

        template <typename RawModel>
        void ISingleSystem<RawModel>::markClean(u32 idx) {
            Singleton<model::SceneStorage<RawModel>>::Get()->setModelStateAtIndex(idx, model::ModelAccessorState::clean);
        }

        template <typename RawModel>
        void ISingleSystem<RawModel>::markChanged(u32 idx) {
            Singleton<model::SceneStorage<RawModel>>::Get()->setModelStateAtIndex(idx, model::ModelAccessorState::changed);
        }

        template <typename RawModel>
        void ISingleSystem<RawModel>::markDeleted(u32 idx) {
            Singleton<model::SceneStorage<RawModel>>::Get()->setModelStateAtIndex(idx, model::ModelAccessorState::deleted);
        }



        template <typename ...RawModels>
        IMultiSystem<RawModels...>::IMultiSystem() {
        }

        template <typename ...RawModels>
        IMultiSystem<RawModels...>::~IMultiSystem() {
        }

        template <typename RawModel, typename ...Rest>
        u64 buildBitmask() {
            u64 thisMask = Singleton<model::SceneStorage<RawModel>>::Get()->getBitmask();
            if constexpr (sizeof...(Rest) == 0) return thisMask;
            else return thisMask | buildBitmask<Rest...>();
        }

        template <typename ...RawModels>
        void IMultiSystem<RawModels...>::onUpdate(f32 frameDt, f32 updateDt) {
            Array<Entity> matches = EntityRegistry::GetMatching(buildBitmask<RawModels...>());
            process(frameDt, updateDt, matches);
        }
    };
};
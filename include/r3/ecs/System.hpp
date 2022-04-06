#pragma once
#include <r3/ecs/System.h>
#include <r3/models/SceneStorage.hpp>
#include <r3/utils/Singleton.hpp>
#include <r3/utils/Array.hpp>

namespace r3 {
    namespace ecs {
        template <typename RawModel>
        ISystem<RawModel>::ISystem() {
        }

        template <typename RawModel>
        ISystem<RawModel>::~ISystem() {
        }

        template <typename RawModel>
        void ISystem<RawModel>::onUpdate(f32 frameDt, f32 updateDt) {
            auto storage = Singleton<model::SceneStorage<RawModel>>::Get();
            updateComponents(frameDt, updateDt, storage->getData());
        }
    };
};
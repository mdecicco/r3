#pragma once

namespace r3 {
    namespace model {
        template <typename Cls>
        class ModelAccessor;

        struct mEntity;
        struct mTransformComponent;
        struct mNameComponent;
    };

    namespace ecs {
        typedef model::ModelAccessor<model::mEntity> aEntity;
        typedef model::ModelAccessor<model::mTransformComponent> aTransformComponent;
        typedef model::ModelAccessor<model::mNameComponent> aNameComponent;
    };
};
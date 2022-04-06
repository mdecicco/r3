#pragma once

namespace r3 {
    namespace model {
        template <typename Cls>
        class ModelAccessor;

        struct RawEntity;
        struct RawTransformComponent;
        struct RawNameComponent;
    };

    namespace ecs {
        typedef model::ModelAccessor<model::RawEntity> aEntity;
        typedef model::ModelAccessor<model::RawTransformComponent> aTransformComponent;
        typedef model::ModelAccessor<model::RawNameComponent> aNameComponent;
    };
};
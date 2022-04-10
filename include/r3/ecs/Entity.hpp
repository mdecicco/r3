#pragma once
#include <r3/ecs/Entity.h>
#include <r3/models/SceneStorage.hpp>

namespace r3 {
    namespace ecs {
        template <typename RawModel>
        std::enable_if_t<model::is_entity_component<RawModel>::value, model::ModelAccessor<RawModel>>
        Entity::addComponent(const RawModel& comp) {
            if (!m_self) return model::ModelAccessor<RawModel>();
            auto storage = Singleton<model::SceneStorage<RawModel>>::Get();
            auto existing = storage->getModelForEntity(m_self->id);

            if (existing) return existing;
            RawModel m = comp;
            m.entityId = m_self->id;
            return storage->submit(m);
        }
        
        template <typename RawModel>
        std::enable_if_t<model::is_entity_component<RawModel>::value, model::ModelAccessor<RawModel>>
        Entity::getComponent() {
            if (!m_self) return model::ModelAccessor<RawModel>();
            return Singleton<model::SceneStorage<RawModel>>::Get()->getModelForEntity(m_self->id);
        }
    };
};
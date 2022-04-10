#pragma once
#include <r3/common/Types.h>
#include <r3/ecs/Types.h>
#include <r3/models/EntityModel.h>
#include <r3/models/SceneStorage.h>
#include <r3/utils/robin_hood.h>
#include <r3/utils/Singleton.h>

namespace r3 {
    namespace ecs {
        class Entity {
            public:
                Entity();
                Entity(const aEntity& e);
                ~Entity();

                model::EntityId getId() const;
                model::EntityId getParentId() const;
                model::SceneId getSceneId() const;
                u64 getComponentMask() const;
                Entity getParent();
                Array<Entity> getChildren();
                model::EntityModel::Instance getRaw() const;

                template <typename RawModel>
                std::enable_if_t<model::is_entity_component<RawModel>::value, model::ModelAccessor<RawModel>>
                addComponent(const RawModel& comp);
                
                template <typename RawModel>
                std::enable_if_t<model::is_entity_component<RawModel>::value, model::ModelAccessor<RawModel>>
                getComponent();

                operator bool() const;

            protected:
                friend class sEntityRegistry;
                aEntity m_self;
                u64 m_componentMask;
        };

        class sEntityRegistry {
            public:
                void onEntityCreated(aEntity e);
                void onComponentAdded(model::EntityId to, u64 compMask);
                void onComponentRemoved(model::EntityId from, u64 compMask);

            protected:
                friend class EntityRegistry;
                Array<Entity> m_entities;
                robin_hood::unordered_map<u64, u32> m_entityIdxMap;
        };

        class EntityRegistry : public Singleton<sEntityRegistry> {
            public:
                static Entity CreateEntity(model::EntityId parentId = 0);
                static Array<Entity> GetAll();
                static Array<Entity> GetMatching(u64 componentBitmask);
                static Entity GetEntity(model::EntityId id);
                static Array<Entity> GetChildrenOf(model::EntityId of);
        };
    };
};
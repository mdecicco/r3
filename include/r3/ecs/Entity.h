#pragma once
#include <r3/common/Types.h>
#include <r3/ecs/Types.h>
#include <r3/models/EntityModel.h>
#include <r3/utils/robin_hood.h>
#include <r3/models/SceneStorage.h>

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
                Entity getParent();
                Array<Entity> getChildren();
                model::RawEntity getRaw() const;

                aTransformComponent addTransformComponent();
                aTransformComponent getTransform() const;
                aNameComponent addNameComponent();
                aNameComponent getName() const;

                operator bool() const;

            protected:
                aEntity m_self;
        };

        Entity createEntity(model::EntityId parentId = 0);
        Array<Entity> getEntities();
        Entity getEntity(model::EntityId id);
        Array<Entity> getEntityChildren(model::EntityId of);
    };
};
#include <r3/ecs/Entity.h>
#include <r3/utils/Singleton.hpp>
#include <r3/utils/Array.hpp>
#include <r3/models/Models.hpp>
#include <r3/models/SceneStorage.hpp>
#include <r3/models/NameComponentModel.h>
#include <r3/models/TransformComponentModel.h>

namespace r3 {
    namespace ecs {
        //
        // Entity
        //

        Entity::Entity() : m_componentMask(0) {
        }

        Entity::Entity(const aEntity& e) : m_componentMask(0) {
            m_self = e;
        }

        Entity::~Entity() {
        }

        model::EntityId Entity::getId() const {
            return m_self->id;
        }

        model::EntityId Entity::getParentId() const {
            if (!m_self) return 0;
            return m_self->parentId;
        }

        model::SceneId Entity::getSceneId() const {
            if (!m_self) return 0;
            return m_self->sceneId;
        }

        u64 Entity::getComponentMask() const{
            return m_componentMask;
        }

        Entity Entity::getParent() {
            if (!m_self || m_self->parentId == 0) return Entity();

            return EntityRegistry::GetEntity(m_self->parentId);
        }

        Array<Entity> Entity::getChildren() {
            if (!m_self) return {};

            return EntityRegistry::GetChildrenOf(m_self->id);
        }

        model::EntityModel::Instance Entity::getRaw() const {
            return *m_self;
        }

        Entity::operator bool() const {
            return m_self->id != 0;
        }



        //
        // sEntityRegistry
        //

        void sEntityRegistry::onEntityCreated(aEntity e) {
            m_entityIdxMap[e->id] = m_entities.size();
            m_entities.emplace(e);
        }

        void sEntityRegistry::onComponentAdded(model::EntityId to, u64 compMask) {
            m_entities[m_entityIdxMap[to]].m_componentMask |= compMask;
        }

        void sEntityRegistry::onComponentRemoved(model::EntityId to, u64 compMask) {
            m_entities[m_entityIdxMap[to]].m_componentMask &= ~compMask;
        }

        //
        // sEntityRegistry
        //

        Entity EntityRegistry::CreateEntity(model::EntityId parentId) {
            auto es = model::EntityModel::Storage();
            return Entity(es->submit({ 0, parentId, es->getSceneId() }));
        }

        Array<Entity> EntityRegistry::GetAll() {
            return Get()->m_entities;
        }

        Array<Entity> EntityRegistry::GetMatching(u64 componentBitmask) {
            return Get()->m_entities.filter([componentBitmask](const Entity& e) {
                return (e.getComponentMask() & componentBitmask) == componentBitmask;
            });
        }

        Entity EntityRegistry::GetEntity(model::EntityId id) {
            if (id == 0) return Entity();

            auto& map = Get()->m_entityIdxMap;
            auto it = map.find(id);
            if (it == map.end()) return Entity();

            return Get()->m_entities[it->second];
        }

        Array<Entity> EntityRegistry::GetChildrenOf(model::EntityId of) {
            if (of == 0) return {};

            return Get()->m_entities.filter([of](const Entity& e) {
                return e.getParentId() == of;
            });
        }
    };
};
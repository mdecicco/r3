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
        // Helpers
        //

        Entity createEntity(model::EntityId parentId) {
            auto es = model::EntityModel::Storage();
            return Entity(es->submit({ 0, parentId, es->getSceneId() }));
        }

        Array<Entity> getEntities() {
            return model::EntityModel::Storage()->getData().map([](const aEntity& e) { return Entity(e); });
        }

        Entity getEntity(model::EntityId id) {
            if (id == 0) return Entity();

            auto e = model::EntityModel::Storage()->getModelForEntity(id);

            if (e) return Entity(e);
            return Entity();
        }

        Array<Entity> getEntityChildren(model::EntityId of) {
            Array<Entity> out;
            if (of == 0) return out;

            model::EntityModel::Storage()->getData().each([&out, of](auto& e) {
                if (e->parentId == of) {
                    out.emplace(e);
                }
            });

            return out;
        }



        //
        // Entity
        //

        Entity::Entity() {
        }

        Entity::Entity(const aEntity& e) {
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

        Entity Entity::getParent() {
            if (!m_self || m_self->parentId == 0) return Entity();

            return getEntity(m_self->parentId);
        }

        Array<Entity> Entity::getChildren() {
            if (!m_self) return {};

            return getEntityChildren(m_self->id);
        }

        model::EntityModel::Instance Entity::getRaw() const {
            return *m_self;
        }

        aTransformComponent Entity::addTransformComponent() {
            if (!m_self) return aTransformComponent();
            auto tcs = model::TransformComponentModel::Storage();
            auto tc = tcs->getModelForEntity(m_self->id);
            if (tc) return tc;

            model::TransformComponentModel::Instance _tc;
            _tc.entityId = m_self->id;
            _tc.cellIdx = 0;
            _tc.position = _tc.scale = vec3(0, 0, 0);
            _tc.rotation = quat();
            return tcs->submit(_tc);
        }

        aTransformComponent Entity::getTransform() const {
            if (!m_self) return aTransformComponent();
            return model::TransformComponentModel::Storage()->getModelForEntity(m_self->id);
        }

        aNameComponent Entity::addNameComponent() {
            if (!m_self) return aNameComponent();
            auto ncs = model::NameComponentModel::Storage();
            auto nc = ncs->getModelForEntity(m_self->id);
            if (nc) return nc;

            model::NameComponentModel::Instance _nc;
            _nc.entityId = m_self->id;
            _nc.name = "";
            return ncs->submit(_nc);
        }

        aNameComponent Entity::getName() const {
            if (!m_self) return aNameComponent();
            return model::NameComponentModel::Storage()->getModelForEntity(m_self->id);
        }

        Entity::operator bool() const {
            return m_self->id != 0;
        }
    };
};
#pragma once
#include <r3/models/SceneStorage.h>
#include <r3/common/Engine.h>
#include <r3/utils/Database.hpp>
#include <r3/utils/Singleton.hpp>
#include <r3/models/SceneModel.h>

namespace r3 {
    namespace model {
        //
        // ModelAccessor
        //

        template <typename Cls>
        ModelAccessor<Cls>::ModelAccessor() : m_storage(nullptr), m_index(0) {
        }

        template <typename Cls>
        ModelAccessor<Cls>::ModelAccessor(const ModelAccessor<Cls>& o) : m_storage(o.m_storage), m_index(o.m_index) {
        }

        template <typename Cls>
        ModelAccessor<Cls>::ModelAccessor(u32 index, SceneStorage<Cls>* storage) : m_storage(storage), m_index(index) {
        }

        template <typename Cls>
        ModelAccessor<Cls>::~ModelAccessor() {
        }

        template <typename Cls>
        void ModelAccessor<Cls>::markClean() {
            m_storage->m_modelStates[m_index] = ModelAccessorState::clean;
        }

        template <typename Cls>
        void ModelAccessor<Cls>::markChanged() {
            m_storage->m_modelStates[m_index] = ModelAccessorState::changed;
        }

        template <typename Cls>
        void ModelAccessor<Cls>::markDeleted() {
            m_storage->m_modelStates[m_index] = ModelAccessorState::deleted;
        }

        template <typename Cls>
        bool ModelAccessor<Cls>::isValid() const {
            return m_storage != nullptr;
        }

        template <typename Cls>
        bool ModelAccessor<Cls>::isClean() const {
            return m_storage->m_modelStates[m_index] == ModelAccessorState::clean;
        }

        template <typename Cls>
        bool ModelAccessor<Cls>::isChanged() const {
            return m_storage->m_modelStates[m_index] == ModelAccessorState::changed;
        }

        template <typename Cls>
        bool ModelAccessor<Cls>::isDeleted() const {
            return m_storage->m_modelStates[m_index] == ModelAccessorState::deleted;
        }

        template <typename Cls>
        ModelAccessor<Cls>::operator bool() const {
            return m_storage != nullptr;
        }

        template <typename Cls>
        Cls& ModelAccessor<Cls>::operator*(void) {
            return m_storage->m_models[m_index];
        }

        template <typename Cls>
        Cls ModelAccessor<Cls>::operator*(void) const {
            return m_storage->m_models[m_index];
        }

        template <typename Cls>
        Cls* ModelAccessor<Cls>::operator->() {
            return &m_storage->m_models[m_index];
        }

        template <typename Cls>
        const Cls* ModelAccessor<Cls>::operator->() const {
            return &m_storage->m_models[m_index];
        }



        //
        // SceneStorage
        //

        template <typename Cls>
        SceneStorage<Cls>::SceneStorage(db::Model<Cls>* model, db::ForeignKeyField* sceneOrEntityFK) : m_model(model), m_isValid(false), m_sceneOrEntityFK(sceneOrEntityFK) {
            m_scene.id = 0;
        }

        template <typename Cls>
        SceneStorage<Cls>::~SceneStorage() {
        }

        template <typename Cls>
        model::SceneId SceneStorage<Cls>::getSceneId() const {
            return m_scene.id;
        }

        template <typename Cls>
        model::RawScene SceneStorage<Cls>::getScene() const {
            return m_scene;
        }

        template <typename Cls>
        const Array<ModelAccessor<Cls>>& SceneStorage<Cls>::getData() const {
            return m_cachedAccessors;
        }

        template <typename Cls>
        Array<ModelAccessor<Cls>>& SceneStorage<Cls>::getData() {
            return m_cachedAccessors;
        }

        template <typename Cls>
        const Array<Cls>& SceneStorage<Cls>::getRawData() const {
            return m_models;
        }

        template <typename Cls>
        void SceneStorage<Cls>::setSceneId(model::SceneId sceneId) {
            m_models.clear(true);
            m_cachedAccessors.clear(true);
            m_modelStates.clear(true);
            m_entityIdxMap.clear();
            m_entityIdxMap.reserve(0);

            m_isValid = true;

            db::Database* Db = Engine::Get()->getDb();
            Array<model::RawScene> results;
        
            if (sceneId) {
                results = Db->select(model::SceneModel::Get(), String::Format("id = %d", sceneId));
                if (results.size() != 1) {
                    Engine::Get()->log(LogCode::ecs_invalid_scene_id, sceneId);
                    m_isValid = false;
                    return;
                }
            }
            else {
                results = Db->select(model::SceneModel::Get(), "is_entry_point = 1");
                if (results.size() != 1) {
                    Engine::Get()->log(LogCode::ecs_no_entry_scene, sceneId);
                    m_isValid = false;
                    return;
                }
            }

            m_scene = results[0];

            String fkTable = m_sceneOrEntityFK->getReference()->getTableName();
            if (fkTable == "tblEntity") {
                m_join = String::Format("LEFT OUTER JOIN tblEntity AS _e ON _e.id = %s.%s", m_model->getTableName(), m_sceneOrEntityFK->getName());
                m_loadFilter = String::Format("_e.scene_id = %d", m_scene.id);
            } else if (fkTable == "tblScene") {
                m_loadFilter = String::Format("%s = %d", m_sceneOrEntityFK->getName(), m_scene.id);
            } else {
                Engine::Get()->log(LogCode::ecs_scene_storage_invalid_foreign_key, m_sceneOrEntityFK->getName(), m_sceneOrEntityFK->getReference()->getTableName());
                m_isValid = false;
            }
        }

        template <typename Cls>
        void SceneStorage<Cls>::load() {
            if (!m_isValid) return;

            db::Database* Db = Engine::Get()->getDb();
            m_models = Db->select(m_model, m_loadFilter, "", 0, 0, { m_join });
            m_modelStates.reserve(m_models.size());
            m_entityIdxMap.reserve(m_models.size());
            m_cachedAccessors.reserve(m_models.size());

            m_models.each([this](const Cls& m, u32 idx) {
                m_modelStates.push(ModelAccessorState::clean);
                m_cachedAccessors.emplace(idx, this);
                m_entityIdxMap[getEntityIdFromInstance(m)] = idx;
            });
        }

        template <typename Cls>
        void SceneStorage<Cls>::save() {
            db::Database* Db = Engine::Get()->getDb();
            m_modelStates.each([this, Db](const ModelAccessorState& state, u32 idx) {
                if (state == ModelAccessorState::changed) {
                    Db->update<Cls>(m_model, m_models[idx]);
                } else if (state == ModelAccessorState::deleted) {
                    // todo
                }
            });
        }

        template <typename Cls>
        ModelAccessor<Cls> SceneStorage<Cls>::getModelForEntity(model::EntityId id) {
            auto it = m_entityIdxMap.find(id);
            if (it == m_entityIdxMap.end()) return ModelAccessor<Cls>();
            return ModelAccessor<Cls>(it->second, this);
        }

        template <typename Cls>
        model::EntityId SceneStorage<Cls>::getEntityIdFromInstance(const Cls& instance) const {
            if constexpr (std::is_same_v<model::RawEntity, Cls>) {
                return instance.id;
            } else if constexpr (is_entity_component<Cls>::value) {
                return instance.entityId;
            } else {
                static_assert("SceneStorage template argument must be either 'model::RawEntity', or a struct that derives from 'model::RawComponentBase'");
            }

            return 0;
        }

        template <typename Cls>
        ModelAccessor<Cls> SceneStorage<Cls>::submit(const Cls& obj) {
            db::Database* Db = Engine::Get()->getDb();
            Cls inserted = obj;
            if (Db->insert(m_model, inserted, true)) {
                u32 idx = this->m_models.size();
                m_models.emplace(inserted);
                m_cachedAccessors.emplace(idx, this);
                m_modelStates.push(ModelAccessorState::clean);
                m_entityIdxMap[this->getEntityIdFromInstance(obj)] = idx;

                return ModelAccessor<Cls>(idx, this);
            }

            return ModelAccessor<Cls>();
        }

        template <typename Cls>
        Array<ModelAccessor<Cls>> SceneStorage<Cls>::submitMany(const Array<Cls>& objs, bool doReturnSubmitted) {
            db::Database* Db = Engine::Get()->getDb();
            Array<Cls> inserted = objs;
            Array<ModelAccessor<Cls>> out;
            if (doReturnSubmitted) out.reserve(inserted.size());

            if (Db->insertMany(m_model, inserted, true)) {
                inserted.each([this, &out, doReturnSubmitted](auto& obj) {
                    u32 idx = this->m_models.size();
                    m_models.emplace(obj);
                    m_cachedAccessors.emplace(idx, this);
                    m_modelStates.push(ModelAccessorState::clean);
                    m_entityIdxMap[this->getEntityIdFromInstance(obj)] = idx;
                    if (doReturnSubmitted) out.emplace(idx, this);
                });
            }

            return out;
        }
    };
};
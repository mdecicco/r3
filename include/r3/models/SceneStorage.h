#pragma once
#include <r3/common/Types.h>
#include <r3/ecs/Types.h>
#include <r3/models/SceneModel.h>
#include <r3/utils/robin_hood.h>
#include <r3/utils/Math.h>

namespace r3 {
    namespace model {
        typedef u32 EntityId;
        struct mEntity;
        struct mComponentBase;

        enum class ModelAccessorState {
            clean,
            changed,
            deleted
        };

        template <typename Cls>
        class SceneStorage;

        template <typename Cls>
        class ModelAccessor {
            public:
                ModelAccessor();
                ModelAccessor(const ModelAccessor<Cls>& o);
                ~ModelAccessor();

                void markClean();
                void markChanged();
                void markDeleted();
                bool isValid() const;
                bool isClean() const;
                bool isChanged() const;
                bool isDeleted() const;

                operator bool() const;
                Cls& operator*(void);
                Cls operator*(void) const;
                Cls* operator->();
                const Cls* operator->() const;

            private:
                ModelAccessor(u32 index, SceneStorage<Cls>* storage);
                friend class SceneStorage<Cls>;
                friend class Array<ModelAccessor<Cls>>; // to allow emplace to call the constructor
                u32 m_index;
                SceneStorage<Cls>* m_storage;
        };

        template <typename Cls>
        class SceneStorage {
            public:
                SceneStorage(db::Model<Cls>* model, db::ForeignKeyField* sceneOrEntityFK, u64 compBitmask);
                ~SceneStorage();
                
                template <typename T = Cls>
                std::enable_if_t<std::is_same_v<T, Cls> && std::is_base_of_v<mComponentBase, Cls>, u64>
                getBitmask() const;
                model::SceneId getSceneId() const;
                model::SceneModel::Instance getScene() const;

                const Array<ModelAccessor<Cls>>& getData() const;
                Array<ModelAccessor<Cls>>& getData();
                const Array<Cls>& getRawData() const;

                // If any elements of the returned array are modified without notifying
                // the storage via 'setEntityState' or 'setEntityStateAtIndex'then the
                // changes may be lost between sessions.
                //
                // Modification of the array itself will result in undefined behavior.
                Array<Cls>& getMutableRawData();

                // Less efficient version of 'setModelStateAtIndex'. Use only when the
                // index of the model in the storage is not known.
                void setModelStateForEntity(model::EntityId id, ModelAccessorState state);

                // Updates the model state at the given storage index.
                void setModelStateAtIndex(u32 idx, ModelAccessorState state);
            
                bool setSceneId(model::SceneId sceneId);
                void load();
                void save();
            
                ModelAccessor<Cls> getModelForEntity(model::EntityId id);

                model::EntityId getEntityIdFromInstance(const Cls& instance) const;

                ModelAccessor<Cls> submit(const Cls& obj);

                Array<ModelAccessor<Cls>> submitMany(const Array<Cls>& objs, bool doReturnSubmitted = true);

            protected:
                friend class ModelAccessor<Cls>;

                model::SceneModel::Instance m_scene;
                db::Model<Cls>* m_model;
                db::ForeignKeyField* m_sceneOrEntityFK;
                Array<ModelAccessor<Cls>> m_cachedAccessors;
                Array<Cls> m_models;
                Array<ModelAccessorState> m_modelStates;
                robin_hood::unordered_map<model::EntityId, u32> m_entityIdxMap;
                String m_join;
                String m_loadFilter;
                bool m_isValid;
                u64 m_compBitmask;
        };
    };
};
#pragma once
#include <r3/common/Types.h>
#include <r3/ecs/Types.h>
#include <r3/models/SceneModel.h>
#include <r3/utils/robin_hood.h>
#include <r3/utils/Math.h>

namespace r3 {
    namespace model {
        typedef u32 EntityId;
        struct RawEntity;

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
                SceneStorage(db::Model<Cls>* model, db::ForeignKeyField* sceneOrEntityFK);
                ~SceneStorage();

                model::SceneId getSceneId() const;
                model::RawScene getScene() const;

                const Array<ModelAccessor<Cls>>& getData() const;
                Array<ModelAccessor<Cls>>& getData();
                const Array<Cls>& getRawData() const;
            
                void setSceneId(model::SceneId sceneId);
                void load();
                void save();
            
                ModelAccessor<Cls> getModelForEntity(model::EntityId id);

                model::EntityId getEntityIdFromInstance(const Cls& instance) const;

                ModelAccessor<Cls> submit(const Cls& obj);

                Array<ModelAccessor<Cls>> submitMany(const Array<Cls>& objs, bool doReturnSubmitted = true);

            protected:
                friend class ModelAccessor<Cls>;

                model::RawScene m_scene;
                db::Model<Cls>* m_model;
                db::ForeignKeyField* m_sceneOrEntityFK;
                Array<ModelAccessor<Cls>> m_cachedAccessors;
                Array<Cls> m_models;
                Array<ModelAccessorState> m_modelStates;
                robin_hood::unordered_map<model::EntityId, u32> m_entityIdxMap;
                String m_join;
                String m_loadFilter;
                bool m_isValid;
        };
    };
};
#include <r3/models/Models.h>
#include <r3/models/EntityModel.h>
#include <r3/models/NameComponentModel.h>
#include <r3/models/TransformComponentModel.h>
#include <r3/models/SceneModel.h>
#include <r3/models/SceneStorage.hpp>

#include <r3/utils/Singleton.hpp>
#include <r3/utils/Database.hpp>

namespace r3 {
    namespace model {
        void initializeModels() {
            SceneModel::Init();
            EntityModel::Init();
            NameComponentModel::Init();
            TransformComponentModel::Init();
        }

        void destroyModels() {
            EntityStorage::Destroy();
            NameComponentStorage::Destroy();
            TransformComponentStorage::Destroy();

            SceneModel::Destroy();
            EntityModel::Destroy();
            NameComponentModel::Destroy();
            TransformComponentModel::Destroy();
        }

        void initializeDatabase(db::Database* db) {
            SceneModel::Get()->initializeDb(db);
            EntityModel::Get()->initializeDb(db);
            NameComponentModel::Get()->initializeDb(db);
            TransformComponentModel::Get()->initializeDb(db);
        }

        void initializeSceneStorage(SceneId sceneId) {
            EntityStorage::Get()->setSceneId(sceneId);
            NameComponentStorage::Get()->setSceneId(sceneId);
            TransformComponentStorage::Get()->setSceneId(sceneId);
        }

        void populateSceneStorage() {
            EntityStorage::Get()->load();
            NameComponentStorage::Get()->load();
            TransformComponentStorage::Get()->load();
        }

        void persistSceneStorage() {
            EntityStorage::Get()->save();
            NameComponentStorage::Get()->save();
            TransformComponentStorage::Get()->save();
        }
    };
};
#include <r3/models/Models.h>

#include <r3/models/SessionModel.h>
#include <r3/models/SceneModel.h>
#include <r3/models/EntityModel.h>
#include <r3/models/NameComponentModel.h>
#include <r3/models/TransformComponentModel.h>

#include <r3/models/SceneStorage.hpp>

#include <r3/utils/Singleton.hpp>
#include <r3/utils/Database.hpp>

#include <r3/models/Models.hpp>

#include <r3/common/Engine.h>

namespace r3 {
    namespace model {
        typedef Singleton<std::vector<PlainModelBase*>> Models;

        void registerModel(PlainModelBase* m) {
            if (!Models::Get()) Models::Create();
            Models::Get()->push_back(m);
        }

        bool initializeModels() {
            for (PlainModelBase* m : *Models::Get()) {
                if (!m->init()) {
                    db::ModelBase* mb = m->getModelBase();
                    Engine::Get()->log(LogCode::ecs_component_model_no_entity_foreign_key, mb->getTableName());
                    return false;
                }
            }

            return true;
        }

        void destroyModels() {
            for (PlainModelBase* m : *Models::Get()) m->destroy();
        }

        void initializeDatabase(db::Database* db) {
            for (PlainModelBase* m : *Models::Get()) m->initializeDb(db);
        }

        bool initializeSceneStorage(SceneId sceneId) {
            for (PlainModelBase* m : *Models::Get()) {
                if (!m->initializeSceneStorage(sceneId)) return false;
            }

            return true;
        }

        void populateSceneStorage() {
            for (PlainModelBase* m : *Models::Get()) m->populateSceneStorage();
        }

        void persistSceneStorage() {
            for (PlainModelBase* m : *Models::Get()) m->persistSceneStorage();
        }
    };
};
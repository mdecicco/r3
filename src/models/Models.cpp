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
        u64 __nextCompBit = 0;

        typedef Singleton<std::vector<PlainModelBase*>> Models;

        void registerModel(PlainModelBase* m) {
            if (!Models::Get()) Models::Create();
            Models::Get()->push_back(m);
        }

        bool initializeModels() {
            printf("Model order:\n");
            auto& models = *Models::Get();
            for (u32 i = 0;i < models.size();i++) {
                printf("%d. %s\n", i + 1, models[i]->getModelBase()->getTableName());
            }

            for (u32 i = 0;i < models.size();i++) {
                PlainModelBase* m = models[i];
                if (!m->init()) {
                    db::ModelBase* mb = m->getModelBase();
                    Engine::Get()->log(LogCode::ecs_component_model_no_entity_foreign_key, mb->getTableName());
                    return false;
                }
            }

            return true;
        }

        void destroyModels() {
            auto& models = *Models::Get();
            for (u32 i = 0;i < models.size();i++) {
                models[i]->destroy();
            }
        }

        void initializeDatabase(db::Database* db) {
            auto& models = *Models::Get();
            for (u32 i = 0;i < models.size();i++) {
                models[i]->initializeDb(db);
            }
        }

        bool initializeSceneStorage(SceneId sceneId) {
            auto& models = *Models::Get();
            for (u32 i = 0;i < models.size();i++) {
                if (!models[i]->initializeSceneStorage(sceneId)) return false;
            }

            return true;
        }

        void populateSceneStorage() {
            auto& models = *Models::Get();
            for (u32 i = 0;i < models.size();i++) {
                models[i]->populateSceneStorage();
            }
        }

        void persistSceneStorage() {
            auto& models = *Models::Get();
            for (u32 i = 0;i < models.size();i++) {
                models[i]->persistSceneStorage();
            }
        }
    };
};
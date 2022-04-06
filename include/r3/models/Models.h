#pragma once
#include <r3/common/Types.h>

namespace r3 {
    namespace db {
        class Database;
    };

    namespace model {
        void initializeModels();
        void destroyModels();
        void initializeDatabase(db::Database* db);

        typedef u32 SceneId;
        void initializeSceneStorage(SceneId sceneId);
        void populateSceneStorage();
        void persistSceneStorage();
    };
};
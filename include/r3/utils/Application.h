#pragma once
#include <r3/common/Types.h>
#include <r3/utils/PeriodicUpdate.h>
#include <r3/utils/Arguments.h>
#include <r3/models/SceneModel.h>

namespace gjs {
    class script_context;
};

namespace r3 {
    namespace db {
        class Database;
    };

    class IApplication : public IPeriodicUpdate {
        public:
            IApplication();
            virtual ~IApplication();

            // Called every time the engine starts, before the engine initializes itself
            // If the engine config must be modified before starting, this is where it
            // should be done.
            virtual void onBoot(const Arguments& args);

            // Called every time the engine starts, after the engine initializes itself
            // but before the script-exposed API has been finalized. Any application
            // specific bindings for scripts should be created here.
            virtual void onScriptBind(gjs::script_context* context);

            // Called every time the engine starts, after the engine initializes itself
            // and before the main loop has been started. If entities are created here,
            // they will not be assigned to any scene unless explicitly assigned after
            // creation.
            virtual void afterInitialize();

            // Called every time a scene's data has been loaded from the database. Any
            // entities created on or after a scene is loaded will be assigned to the
            // current scene unless explicitly reassigned after creation.
            virtual void onSceneLoaded(const model::SceneModel::Instance& scene);

            // Called when the engine has been run for the first time with any database
            // after initialization, but before 'afterInitialize' is called.
            virtual void onCreate(db::Database* freshDb);

            // Called periodically, depending on specified update rate. Called at 60 Hz
            // by default, if enabled by the user via 'enableUpdates'.
            virtual void onUpdate(f32 frameDt, f32 updateDt);

            // Called after the main loop breaks but before any shutdown procedures are
            // started.
            virtual void beforeShutdown();
    };
};
#include <r3/utils/Application.h>

namespace r3 {
    IApplication::IApplication() { }

    IApplication::~IApplication() { }

    void IApplication::onBoot(const Arguments& args) { }

    void IApplication::onScriptBind(gjs::script_context* context) { }

    void IApplication::afterInitialize() { }

    void IApplication::onSceneLoaded(const model::SceneModel::Instance& scene) { }

    void IApplication::onCreate(db::Database* freshDb) { }

    void IApplication::onUpdate(f32 frameDt, f32 updateDt) { }

    void IApplication::beforeShutdown() { }
};
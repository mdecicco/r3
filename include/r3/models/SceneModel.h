#pragma once
#include <r3/common/Types.h>
#include <r3/models/Models.h>

namespace r3 {
    namespace model {
        typedef u32 SceneId;
        typedef u32 SceneCellId;

        struct RawScene {
            SceneId id;
            String name;
            bool isEntryPoint;

            struct _dims {
                i32 width;
                i32 height;
                i32 length;
            } dims;

            struct _divs {
                i32 width;
                i32 height;
                i32 length;
            } divs;
        };

        class SceneModel : public model::ModelBase<RawScene, "tblScene", SceneModel> {
            public:
                PrimaryKey<"id", &RawScene::id> id;
                String<"name", &RawScene::name, 64> name;
                Boolean<"is_entry_point", &RawScene::isEntryPoint> isEntryPoint;

                struct {
                    Integer<"width", offsetof(RawScene, dims) + offsetof(RawScene::_dims, width)> width;
                    Integer<"height", offsetof(RawScene, dims) + offsetof(RawScene::_dims, height)> height;
                    Integer<"length", offsetof(RawScene, dims) + offsetof(RawScene::_dims, length)> length;
                } dims;

                struct {
                    Integer<"width_divs", offsetof(RawScene, divs) + offsetof(RawScene::_divs, width)> width;
                    Integer<"height_divs", offsetof(RawScene, divs) + offsetof(RawScene::_divs, height)> height;
                    Integer<"length_divs", offsetof(RawScene, divs) + offsetof(RawScene::_divs, length)> length;
                } divs;

                virtual void initModel(db::Model<RawScene>* self);
        };
    };
};
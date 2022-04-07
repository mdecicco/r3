#pragma once
#include <r3/common/Types.h>
#include <r3/models/Models.h>

namespace r3 {
    namespace model {
        typedef u32 SceneId;
        typedef u32 SceneCellId;

        struct mScene {
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

        class SceneModel : public model::ModelBase<mScene, "tblScene", SceneModel> {
            public:
                PrimaryKey<"id", &Instance::id> id;
                String<"name", &Instance::name, 64> name;
                Boolean<"is_entry_point", &Instance::isEntryPoint> isEntryPoint;

                struct {
                    Integer<"width", offsetof(Instance, dims) + offsetof(Instance::_dims, width)> width;
                    Integer<"height", offsetof(Instance, dims) + offsetof(Instance::_dims, height)> height;
                    Integer<"length", offsetof(Instance, dims) + offsetof(Instance::_dims, length)> length;
                } dims;

                struct {
                    Integer<"width_divs", offsetof(Instance, divs) + offsetof(Instance::_divs, width)> width;
                    Integer<"height_divs", offsetof(Instance, divs) + offsetof(Instance::_divs, height)> height;
                    Integer<"length_divs", offsetof(Instance, divs) + offsetof(Instance::_divs, length)> length;
                } divs;

                virtual void initModel(db::Model<Instance>* self);
        };
    };
};
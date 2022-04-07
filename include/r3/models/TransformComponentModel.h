#pragma once
#include <r3/common/Types.h>
#include <r3/models/EntityModel.h>
#include <r3/utils/Math.h>

namespace r3 {
    namespace model {
        struct mTransformComponent : mComponentBase {
            i32 cellIdx;

            vec3 position;
            vec3 scale;
            quat rotation;
        };

        class TransformComponentModel : public ModelBase<mTransformComponent, "tblTransformComponent", TransformComponentModel> {
            public:
                ForeignKey<"entity_id", &Instance::entityId, &EntityModel::id, false, true> entityId;
                Integer<"cell_idx", &Instance::cellIdx, false, false, "cell_idx > 0 OR cell_idx = -1"> cellIdx;

                struct {
                    Float<"pos_x", offsetof(Instance, position) + 0> x;
                    Float<"pos_y", offsetof(Instance, position) + 4> y;
                    Float<"pos_z", offsetof(Instance, position) + 8> z;
                } pos;

                struct {
                    Float<"scale_x", offsetof(Instance, scale) + 0> x;
                    Float<"scale_y", offsetof(Instance, scale) + 4> y;
                    Float<"scale_z", offsetof(Instance, scale) + 8> z;
                } scale;

                struct {
                    Float<"rot_x", offsetof(Instance, rotation) + 0> x;
                    Float<"rot_y", offsetof(Instance, rotation) + 4> y;
                    Float<"rot_z", offsetof(Instance, rotation) + 8> z;
                    Float<"rot_w", offsetof(Instance, rotation) + 12> w;
                } rot;
        };
    };
};
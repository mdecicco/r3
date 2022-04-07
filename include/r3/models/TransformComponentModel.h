#pragma once
#include <r3/common/Types.h>
#include <r3/models/EntityModel.h>
#include <r3/utils/Math.h>

namespace r3 {
    namespace model {
        struct RawTransformComponent : RawComponentBase {
            i32 cellIdx;

            vec3 position;
            vec3 scale;
            quat rotation;
        };

        class TransformComponentModel : public ModelBase<RawTransformComponent, "tblTransformComponent", TransformComponentModel> {
            public:
                using Raw = RawTransformComponent;
                using Self = TransformComponentModel;

                ForeignKey<"entity_id", &Raw::entityId, &EntityModel::id, false, true> entityId;
                Integer<"cell_idx", &Raw::cellIdx, false, false, "cell_idx > 0 OR cell_idx = -1"> cellIdx;

                struct {
                    Float<"pos_x", offsetof(Raw, position) + 0> x;
                    Float<"pos_y", offsetof(Raw, position) + 4> y;
                    Float<"pos_z", offsetof(Raw, position) + 8> z;
                } pos;

                struct {
                    Float<"scale_x", offsetof(Raw, scale) + 0> x;
                    Float<"scale_y", offsetof(Raw, scale) + 4> y;
                    Float<"scale_z", offsetof(Raw, scale) + 8> z;
                } scale;

                struct {
                    Float<"rot_x", offsetof(Raw, rotation) + 0> x;
                    Float<"rot_y", offsetof(Raw, rotation) + 4> y;
                    Float<"rot_z", offsetof(Raw, rotation) + 8> z;
                    Float<"rot_w", offsetof(Raw, rotation) + 12> w;
                } rot;
        };
    };
};
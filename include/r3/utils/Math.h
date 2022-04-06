#pragma once
#include <r3/common/Types.h>

namespace r3 {
    typedef f32 angle;

    class vec2 {
        public:
            vec2();
            vec2(f32 xy);
            vec2(f32 x, f32 y);
            
            f32 x, y;
    };
    
    class vec3 {
        public:
            vec3();
            vec3(f32 xyz);
            vec3(f32 x, f32 y, f32 z);
            
            f32 x, y, z;
    };
    
    class vec4 {
        public:
            vec4();
            vec4(f32 xyzw);
            vec4(f32 x, f32 y, f32 z, f32 w);
            vec4(const vec3& xyz, f32 z);
            
            f32 x, y, z, w;
    };
    
    class quat {
        public:
            quat();
            quat(f32 x, f32 y, f32 z, f32 w);
            quat(const vec3& axis, angle degrees);
            
            f32 x, y, z, w;
    };
};
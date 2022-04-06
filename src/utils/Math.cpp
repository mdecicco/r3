#include <r3/utils/Math.h>
#include <cmath>

namespace r3 {
    //
    // vec2
    //

    vec2::vec2() : x(0.0f), y(0.0f) { }
    vec2::vec2(f32 xy) : x(xy), y(xy) { }
    vec2::vec2(f32 _x, f32 _y) : x(_x), y(_y) { }



    //
    // vec3
    //

    vec3::vec3() : x(0.0f), y(0.0f), z(0.0f) { }
    vec3::vec3(f32 xyz) : x(xyz), y(xyz), z(xyz) { }
    vec3::vec3(f32 _x, f32 _y, f32 _z) : x(_x), y(_y), z(_z) { }



    //
    // vec4
    //

    vec4::vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
    vec4::vec4(f32 xyzw) : x(xyzw), y(xyzw), z(xyzw), w(xyzw) { }
    vec4::vec4(f32 _x, f32 _y, f32 _z, f32 _w) : x(_x), y(_y), z(_z), w(_w) { }
    vec4::vec4(const vec3& xyz, f32 _w) : x(xyz.x), y(xyz.y), z(xyz.z), w(_w) { }



    //
    // quat
    //

    quat::quat() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
    quat::quat(f32 _x, f32 _y, f32 _z, f32 _w) : x(_x), y(_y), z(_z), w(_w) { }
    quat::quat(const vec3& axis, angle degrees) {
        f32 Hrad = (degrees * 0.5f) * (3.14159265358979323846f / 180.0f);
        f32 sHrad = sin(Hrad);
        x = axis.x * sHrad;
        y = axis.y * sHrad;
        z = axis.z * sHrad;
        w = cos(Hrad);
    }
};
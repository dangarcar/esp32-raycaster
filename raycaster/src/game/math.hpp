#pragma once
 
#include <math.h>
#include "system.h"

#define DEG_TO_RAD 0.017453292519943295769236907684886
#define radians(deg) ((deg)*DEG_TO_RAD)
constexpr float M_2PI = M_PI * 2.f;
#define max(a,b) ((a)>(b)?(a):(b))

struct Vector2 {
    float x,y;

    Vector2() = default;
    Vector2(float x, float y): x{x}, y{y} {};
};

inline Vector2 operator*(const Vector2& v, float scalar) {
    return Vector2(v.x*scalar, v.y*scalar);
}

inline void operator+=(Vector2& a, const Vector2& b) {
    a.x += b.x;
    a.y += b.y;
}

inline void operator*=(Vector2& a, float scalar) {
    a.x *= scalar;
    a.y *= scalar;
}

void rotate(Vector2& v, float angle) {
    float theta = radians(angle);

    float cs = cosf(theta);
    float sn = sinf(theta);

    v.x = v.x * cs - v.y * sn;
    v.y = v.x * sn + v.y * cs;
}

inline Vector2 rotated90Deg(Vector2 v) {
    return Vector2(v.y, -v.x);
}

inline Vector2 rotatedMinus90Deg(Vector2 v) {
    return Vector2(-v.y, v.x);
}

inline Vector2 invert(Vector2 v) {
    return Vector2(-v.x, -v.y);
}

Vector2 normalized(Vector2 v) {
    float l = sqrtf(v.x*v.x + v.y*v.y);
    return Vector2(v.x/l, v.y/l);
}

float length(Vector2 v) {
    return sqrtf(v.x*v.x + v.y*v.y);
}

float dot(Vector2 a, Vector2 b) {
    return a.x*b.x + a.y*b.y;
}
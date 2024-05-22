#pragma once
struct Vector4 {
    float x, y, z, w;
};
struct Vector3 {
    float x, y, z;
};
struct Matrix4x4 {
    float m[4][4];
};
struct Transform {
    Vector3 scale;
    Vector3 rotate;
    Vector3 translate;
};
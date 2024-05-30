#pragma once
struct Vector4 {
    float x, y, z, w;
};
struct Vector3 {
    float x, y, z;
};
struct Vector2 {
    float x, y;
};
struct Matrix4x4 {
    float m[4][4];
};
struct Transform {
    Vector3 scale;
    Vector3 rotate;
    Vector3 translate;
};
struct VertexData
{
    Vector4 position;
    Vector2 texcoord;
};
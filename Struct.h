#pragma once
#include <vector>
#include <string>
struct Vector4 {
    float x, y, z, w;
};
struct Vector3 {
    float x, y, z;

     Vector3 operator*(float scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }

    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
};
struct Vector2 {
    float x, y;
};
struct Matrix3x3 {
     float m[3][3];
};
struct Matrix4x4 {
    float m[4][4];

     Vector4 operator*(const Vector4& vec) const {
        Vector4 result;
        result.x = m[0][0] * vec.x + m[1][0] * vec.y + m[2][0] * vec.z + m[3][0] * vec.w;
        result.y = m[0][1] * vec.x + m[1][1] * vec.y + m[2][1] * vec.z + m[3][1] * vec.w;
        result.z = m[0][2] * vec.x + m[1][2] * vec.y + m[2][2] * vec.z + m[3][2] * vec.w;
        result.w = m[0][3] * vec.x + m[1][3] * vec.y + m[2][3] * vec.z + m[3][3] * vec.w;
        return result;
    }
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
    Vector3 normal;
};
struct TriangleData {
    float color[4];
    Transform transform;
};
struct Material {
    Vector4 color;
    int32_t enableLighting;
    float padding[3];
    Matrix4x4 uvTransform;
};
struct TransformationMatrix {
    Matrix4x4 WVP;
    Matrix4x4 World;
};
struct DirectionalLight {
    Vector4 color;
    Vector3 direction;
    float intensity;
};
struct MateriaData
{
    std::string textureFilePath;
};
struct ModelData {
    std::vector<VertexData>vertices;
    MateriaData material;
};
struct Particle
{
    Transform transform;
    Vector3 velocity;
    Vector4 color;
    float lifeTime;
    float currentTime;
};
struct ParticleForGPU
{
   Matrix4x4 WVP;
    Matrix4x4 World;
    Vector4 color;
};

struct Emitter {
	Transform transform;
	uint32_t count;//発生数
	float frequency;//発生頻度
	float frequencyTime;//頻度用時刻
};
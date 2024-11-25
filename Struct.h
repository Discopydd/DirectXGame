#pragma once
#include <vector>
#include <string>
struct VertexData
{
    Vector4 position;
    Vector2 texcoord;
    Vector3 normal;
};
//struct TriangleData {
//    float color[4];
//    Transform transform;
//};
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


struct AABB {
	Vector3 min;//最小点
	Vector3 max;//最大点
};
struct AccelerationField {
	Vector3 acceleration;//加速度
	AABB area;//範囲
};
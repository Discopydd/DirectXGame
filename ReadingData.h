#pragma once

#include "math/Matrix4x4.h"
#include "math/Matrix3x3.h"
#include "math/Vector4.h"
#include "math/Vector3.h"
#include "math/Vector2.h"

#include <cstdint>
#include <string>
#include <vector>


struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
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

struct MaterialData {
	std::string textureFilePath;
	uint32_t textureIndex = 0;
};

struct ModelData {
	std::vector<VertexData>vertices;
	MaterialData material;
};
#pragma once
#include "Sprite.h"
#include "MaterialData.h"
#include <vector>

struct ModelData {
	std::vector<VertexData>vertices;
	MaterialData material;
};
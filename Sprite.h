#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include"WinApp.h"
#include <wrl/client.h>
#include <d3d12.h>

//頂点データ
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};
//マテリアルデータ
struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};
//座標変換行列データ
struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};
struct Transform {
    Vector3 scale;
    Vector3 rotate;
    Vector3 translate;
};
class SpriteCommon;
class Sprite
{
public:
	//初期化
	void Initialize(SpriteCommon*spriteCommon);
    //更新
	void Update();
	//描画
	void Draw();
private:
	SpriteCommon* spriteCommon = nullptr;
	//頂点データ作成
	void VertexDataCreate();
	//index作成
	void IndexCreate();
	//マテリアル作成
	void MaterialCreate();
	//座標変換行列データ作成
	void TransformationCreate();
	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	Material* materialData = nullptr;
	TransformationMatrix* transformationMatrixData = nullptr;
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
};

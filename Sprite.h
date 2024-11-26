#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "externals/DirectXTex/d3dx12.h"

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
	void Initialize(SpriteCommon*spriteCommon,std::string textureFilePath);
    //更新
	void Update();
	//描画
	void Draw();
	//テクスチャ変更
	void SetTexture(std::string textureFilePath);

	///getter///
	//位置
	const Vector2& GetPosition()const { return position; }
	//回転
	float GetRotation()const { return rotation; }
	//色
	const Vector4& GetColor()const { return materialData->color; }
	//サイズ
	const Vector2& GetSize()const { return size; }
	//アンカーポイント
	const Vector2& GetAnchorPoint()const { return anchorPoint; }
	//テクスチャ左上座標
	const Vector2& GettextureLeftTop()const { return textureLeftTop; }
	//テクスチャ切り出しサイズ
	const Vector2& GettextureSize()const { return textureSize; }
	//左右フリップ
	const bool& GetisFlipX()const { return isFlipX_; }
	//上下フリップ
	const bool& GetisFlipY()const { return isFlipY_; }

	///setter///
	//位置
	void SetPosition(const Vector2& position) { this->position = position; }
	//回転
	void SetRotation(float rotation) { this->rotation = rotation; }
	//色
	void SetColor(const Vector4& color) { materialData->color = color; }
	//サイズ
	void SetSize(const Vector2& size) { this->size = size; }
	//アンカーポイント
	void SetAnchorPoint(const Vector2& anchorPoint) { this->anchorPoint = anchorPoint; }
	//テクスチャ左上座標
	void SettextureLeftTop(const Vector2& textureLeftTop) { this->textureLeftTop = textureLeftTop; }
	//テクスチャ切り出しサイズ
	void SettextureSize(const Vector2& textureSize) { this->textureSize = textureSize; }
	//左右フリップ
	void SetisFlipX(bool isFlipX) { this->isFlipX_ = isFlipX; }
	//上下フリップ
	void SetisFlipY(bool isFlipY) { this->isFlipY_ = isFlipY; }
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
	//テクスチャサイズをイメージに合わせる
	void AdjustTextureSize();
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

	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
	//Sprite個々の座標
	Vector2 position = { 0.0f,0.0f };
	//サイズ
	Vector2 size = { 640.0f,360.0f };
	//アンカーポイント
	Vector2 anchorPoint = { 0.0f,0.0f };
	//テクスチャ左上座標
	Vector2 textureLeftTop = { 0.0f,0.0f };
	//テクスチャ切り出しサイズ
	Vector2 textureSize = { 100.0f,100.0f };
    //Sprite個々の回転角
	float rotation = 0.0f;
	//テクスチャ番号
	uint32_t textureIndex = 0;
	//左右フリップ
	bool isFlipX_ = false;
	//上下フリップ
	bool isFlipY_ = false;
};

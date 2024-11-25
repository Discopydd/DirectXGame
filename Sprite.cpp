#include "Sprite.h"
#include "SpriteCommon.h"
#include"MyMath.h"

void Sprite::Initialize(SpriteCommon* spriteCommon)
{
	this->spriteCommon = spriteCommon;
	VertexDataCreate();
	IndexCreate();
	MaterialCreate();
	TransformationCreate();
}

void Sprite::Update()
{
	// 頂点リソースにデータを書き込む
	vertexData[0].position = { 0.0f,360.0f,0.0f,1.0f };// 左下
	vertexData[0].texcoord = { 0.0f,1.0f };
	vertexData[0].normal = { 0.0f,0.0f,-1.0f };
	vertexData[1].position = { 0.0f,0.0f,0.0f,1.0f };// 左上
	vertexData[1].texcoord = { 0.0f,0.0f };
	vertexData[1].normal = { 0.0f,0.0f,-1.0f };
	vertexData[2].position = { 640.0f,360.0f,0.0f,1.0f };// 右下
	vertexData[2].texcoord = { 1.0f,1.0f };
	vertexData[2].normal = { 0.0f,0.0f,-1.0f };
	vertexData[3].position = { 640.0f,0.0f,0.0f,1.0f };// 左上
	vertexData[3].texcoord = { 1.0f,0.0f };
	vertexData[3].normal = { 0.0f,0.0f,-1.0f };
	//インデックスリソースにデータを書き込む
	indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
	indexData[3] = 1; indexData[4] = 3; indexData[5] = 2;

	//Transform情報を作る
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//TransformからWorldMatrixを作る
	Matrix4x4 worldMatrix = Math::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	//ViewMatrixを作って単位行列を代入
	Matrix4x4 viewMatrix = Math::MakeIdentity4x4();
	//ProjectionMatrixを作って平行投影行列を書き込む
	Matrix4x4 projectionMatrixSprite = Math::MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = Math::Multiply(worldMatrix, Math::Multiply(viewMatrix, projectionMatrixSprite));
	transformationMatrixData->WVP = worldViewProjectionMatrixSprite;
	transformationMatrixData->World = worldMatrix;
}

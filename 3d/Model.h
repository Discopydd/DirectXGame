#pragma once
#include "ModelCommon.h"
#include "MaterialData.h"
#include"ModelData.h"
#include"Material.h"
#include"../VertexData.h"
class Model
{
public:
	// 初期化
	void Initialize(ModelCommon* modeleCommon, const std::string& directorypath, const std::string& filename);

	// 描画
	void Draw();

	static MaterialData LoadMaterialTemplateFile(const std::string& directorypath, const std::string& filename);
	static ModelData LoadObjectFile(const std::string& ditrectoryPath, const std::string& filename);

private:
	// モデル
	ModelCommon* modelCommon_ = nullptr;
	//objファイルのデータ
	ModelData modelData;
	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	// バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	// マテリアル用のリソースを作る。今回color1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	// マテリアルにデータを書き込む	
	Material* materialData = nullptr;
};
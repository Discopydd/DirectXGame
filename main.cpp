#include<Windows.h>
#include <sstream>
#include<fstream>
#include<string>
#include<format>
#include <wrl.h>
#include<random>
#include <algorithm>

#include"math/MyMath.h"
#include"math/Vector3.h"
#include"MaterialData.h"
#include"ModelData.h"
#include"DebugReporter.h"
#include"Input.h"
#include"WinApp.h"
#include <numbers>
#include "Logger.h"
#include "DirectXCommon.h"
#include "D3DResourceLeakChecker.h"
#include "DirectionalLight.h"
#include <corecrt_math_defines.h>
#include "math/Transform.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "TextureManager.h"

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
    MaterialData materiaData;
    std::string line;
    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open());

    while (std::getline(file,line))
    {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;

        if (identifier == "map_Kd") {
            std::string textureFilename;
            s >> textureFilename;
            materiaData.textureFilePath = directoryPath + "/" + textureFilename;
        }
    }
    return materiaData;
}

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
    ModelData modelData;
    std::vector<Vector4> positions;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;
    std::string line;

    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open());  // 确保文件成功打开


    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string identifier;
        lineStream >> identifier;

        if (identifier == "v") {
            Vector4 position;
            lineStream >> position.x >> position.y >> position.z;
            position.w = 1.0f;
            position.x *= -1.0f;
            positions.push_back(position);
        }
        else if (identifier == "vt") {
            Vector2 texcoord;
            lineStream >> texcoord.x >> texcoord.y;
             texcoord.y = 1.0f - texcoord.y;
            texcoords.push_back(texcoord);
        }
        else if (identifier == "vn") {
            Vector3 normal;
            lineStream >> normal.x >> normal.y >> normal.z;
            normal.x *= -1.0f;
            normals.push_back(normal);
        }
        else if (identifier == "f") {
        VertexData triangle[3];
            for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
                std::string vertexDefintion;
                lineStream >> vertexDefintion;

                std::istringstream v(vertexDefintion);
                uint32_t elementIndices[3];
                for (int32_t element = 0; element < 3; ++element) {
                    std::string index;
                    std::getline(v, index, '/');
                    elementIndices[element] = std::stoi(index);
                }

                Vector4 position = positions[elementIndices[0] - 1];
                Vector2 texcoord = texcoords[elementIndices[1] - 1];
                Vector3 normal = normals[elementIndices[2] - 1];
                triangle[faceVertex] = { position, texcoord, normal };
            }
            modelData.vertices.push_back(triangle[2]);
            modelData.vertices.push_back(triangle[1]);
            modelData.vertices.push_back(triangle[0]);
        }else if (identifier == "mtllib") {

            std::string materialFilename;
            lineStream >> materialFilename;
            modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
    }
    }
     return modelData;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {


    WinApp* winApp = nullptr;
    DirectXCommon* dxCommon = nullptr;
	Input* input = nullptr;

    //Windowの生成
    winApp = new WinApp();
    winApp->Initialize();

    // DX初期化
	dxCommon = new DirectXCommon;
	dxCommon->Initialize(winApp);

	// 入力初期化
	input = new Input();
	input->Initialize(winApp);

    SpriteCommon* spriteCommon = nullptr;
    spriteCommon = new SpriteCommon;
    spriteCommon->Initialize(dxCommon);

    //テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon);



    // WVP用のリソースを作る。 Matrix4x41つ分のサイズを用意する
    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = dxCommon->CreateBufferResource(sizeof(TransformationMatrix));


   //マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = dxCommon->CreateBufferResource(sizeof(Material));
    //マテリアルにデータを書き込む
    Material* materialData = nullptr;
    // 書き込むためのアドレスを取得
    materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

    *materialData = Material{ {1.0f, 1.0f, 1.0f, 1.0f},{1}
    };

    materialData->enableLighting = false;

    materialData->uvTransform =  Math::MakeIdentity4x4();

    //マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite = dxCommon->CreateBufferResource(sizeof(Material));
    //マテリアルにデータを書き込む
    Material* materialDataSprite = nullptr;
    // 書き込むためのアドレスを取得
    materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));

    *materialDataSprite = Material{ {1.0f, 1.0f, 1.0f, 1.0f},{0}
    };

    materialDataSprite->enableLighting = false;

    materialDataSprite->uvTransform =  Math::MakeIdentity4x4();



    Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceModel = dxCommon->CreateBufferResource(sizeof(Material));
    //マテリアルにデータを書き込む
    Material* materialDataModel = nullptr;
    // 書き込むためのアドレスを取得
    materialResourceModel->Map(0, nullptr, reinterpret_cast<void**>(&materialDataModel));

    *materialDataModel = Material{ {1.0f, 1.0f, 1.0f, 1.0f},{1}
    };

    materialDataModel->enableLighting = false;

    materialDataModel->uvTransform =  Math::MakeIdentity4x4();
    // データを書き込む
    TransformationMatrix* wvpData = nullptr;
    wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
    wvpData->World =  Math::MakeIdentity4x4(); // 単位行列を書きこんでおく
    wvpData->WVP =  Math::MakeIdentity4x4();

    Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = dxCommon->CreateBufferResource(sizeof(DirectionalLight));

    // 初始化平行光源的数据
    DirectionalLight* directionalLightData = nullptr;
    directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
    directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    directionalLightData->direction = { 0.0f, -1.0f, 0.0f };
    directionalLightData->intensity = 1.0f;

    Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResourceModel = dxCommon->CreateBufferResource(sizeof(DirectionalLight));

    // 初始化平行光源的数据
    DirectionalLight* directionalLightDataModel = nullptr;
    directionalLightResourceModel->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightDataModel));
    directionalLightDataModel->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    directionalLightDataModel->direction = { 0.0f, -1.0f, 0.0f };
    directionalLightDataModel->intensity = 1.0f;


    // 定义模型的变换数据
    TransformationMatrix* modelWvpData = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> modelWvpResource = dxCommon->CreateBufferResource(sizeof(TransformationMatrix));

    modelWvpResource->Map(0, nullptr, reinterpret_cast<void**>(&modelWvpData));
    modelWvpData->World =  Math::MakeIdentity4x4();
    modelWvpData->WVP =  Math::MakeIdentity4x4();



#pragma region 球

    const uint32_t kSubdivision = 16; // 分割数
    const uint32_t numVertices = (kSubdivision + 1) * (kSubdivision + 1);
    const uint32_t numIndices = kSubdivision * kSubdivision * 6;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = dxCommon->CreateBufferResource(sizeof(VertexData) * numVertices);

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
    vertexBufferView.SizeInBytes = sizeof(VertexData) * numVertices;
    vertexBufferView.StrideInBytes = sizeof(VertexData);
    //頂点リソースにデータを書き込む
    VertexData* vertexData = nullptr;
    // 書き込むためのアドレスを取得
    vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

    const float kLonEvery = 2 * (float)M_PI / (float)kSubdivision; // 经度分割1つの角度
    const float kLatEvery = (float)M_PI / (float)kSubdivision; // 纬度分割1つの角度

    uint32_t vertexIndex = 0;
    for (uint32_t latIndex = 0; latIndex <= kSubdivision; ++latIndex) {
        float lat = -(float)M_PI / 2.0f + kLatEvery * latIndex; // 現在の緯度
        for (uint32_t lonIndex = 0; lonIndex <= kSubdivision; ++lonIndex) {
            float lon = lonIndex * kLonEvery; // 現在の経度

            vertexData[vertexIndex].position.x = cos(lat) * cos(lon);
            vertexData[vertexIndex].position.y = sin(lat);
            vertexData[vertexIndex].position.z = cos(lat) * sin(lon);
            vertexData[vertexIndex].position.w = 1.0f;
            vertexData[vertexIndex].texcoord.x = (float)lonIndex / (float)kSubdivision;
            vertexData[vertexIndex].texcoord.y = 1.0f - (float)latIndex / (float)kSubdivision;
            vertexData[vertexIndex].normal.x = vertexData[vertexIndex].position.x;
            vertexData[vertexIndex].normal.y = vertexData[vertexIndex].position.y;
            vertexData[vertexIndex].normal.z = vertexData[vertexIndex].position.z;
            ++vertexIndex;
        }
    }
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = dxCommon->CreateBufferResource(sizeof(uint32_t) * numIndices);
    D3D12_INDEX_BUFFER_VIEW indexBufferView{};
    indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
    indexBufferView.SizeInBytes = sizeof(uint32_t) * numIndices;
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;

    uint32_t* indexData = nullptr;
    indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

    uint32_t index = 0;
    for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
        for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
            uint32_t first = (latIndex * (kSubdivision + 1)) + lonIndex;
            uint32_t second = first + kSubdivision + 1;

            indexData[index++] = first;
            indexData[index++] = second;
            indexData[index++] = first + 1;

            indexData[index++] = second;
            indexData[index++] = second + 1;
            indexData[index++] = first + 1;
        }
    }

    Transform transform = {};
    transform.scale = { 0.0f,0.0f, 0.0f };
    transform.rotate = { 0.0f, 0.0f, 0.0f };
    transform.translate = { 0.0f, 0.0f, 0.0f };

    Transform cameraTransform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -5.0f} };
#pragma endregion


#pragma region モデル
    // モデル読み込み
    ModelData modelData = LoadObjFile("Resources", "axis.obj");

    // 頂点リソースを作成
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceModel = dxCommon->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());

    // 頂点バッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW vertexBufferViewModel{};
    vertexBufferViewModel.BufferLocation = vertexResourceModel->GetGPUVirtualAddress();
    vertexBufferViewModel.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
    vertexBufferViewModel.StrideInBytes = sizeof(VertexData);

    // 頂点リソースにデータを書き込む
    VertexData* vertexDataModel = nullptr;
    vertexResourceModel->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataModel));
    std::memcpy(vertexDataModel, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

    Transform transformModel = {};
    transformModel.scale = { 1.0f, 1.0f, 1.0f };
    transformModel.rotate = { 0.0f, 0.0f, 0.0f };
    transformModel.translate = { 0.0f, 0.0f, 0.0f };
#pragma endregion
    

 //   // Textureを読んで転送する
 //   DirectX::ScratchImage mipImages = dxCommon->LoadTexture("Resources/uvChecker.png");
 //   const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
 //   Microsoft::WRL::ComPtr<ID3D12Resource> textureResource =  dxCommon->CreateTextureResource(metadata);
 //   Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource =  dxCommon->UploadTextureData(textureResource, mipImages);

 //   // 2枚Textureを読んで転送する
 //   DirectX::ScratchImage mipImages2 =  dxCommon->LoadTexture("Resources/monsterBall.png");
 //   const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
 //   Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2 =  dxCommon->CreateTextureResource(metadata2);
 //   Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource2 =  dxCommon->UploadTextureData(textureResource2, mipImages2);

 //   DirectX::ScratchImage mipImages3 =  dxCommon->LoadTexture(modelData.material.textureFilePath);
 //   const DirectX::TexMetadata& metadata3 = mipImages3.GetMetadata();
 //   Microsoft::WRL::ComPtr<ID3D12Resource> textureResource3 =  dxCommon->CreateTextureResource(metadata3);
 //   Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource3 =  dxCommon->UploadTextureData(textureResource3, mipImages3);

 //   // metaDataを基にSRVの設定
 //   D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
 //   srvDesc.Format = metadata.format;
 //   srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
 //   srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
 //   srvDesc.Texture2D.MipLevels = static_cast<UINT>(metadata.mipLevels);
 //   D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2 = {};
 //   srvDesc2.Format = metadata.format;
 //   srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
 //   srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
 //   srvDesc2.Texture2D.MipLevels = static_cast<UINT>(metadata.mipLevels);
 //   D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc3 = {};
 //   srvDesc3.Format = metadata.format;
 //   srvDesc3.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
 //   srvDesc3.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
 //   srvDesc3.Texture2D.MipLevels = static_cast<UINT>(metadata.mipLevels);
 //   //SRVを作成する DescriptorHeapの場所を決める
 //    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon->GetSRVCPUDescriptorHandle(1);
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = dxCommon->GetSRVGPUDescriptorHandle(1);

	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = dxCommon->GetSRVCPUDescriptorHandle(2);
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = dxCommon->GetSRVGPUDescriptorHandle(2);

	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU3 = dxCommon->GetSRVCPUDescriptorHandle(3);
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU3 = dxCommon->GetSRVGPUDescriptorHandle(3);

 //  textureSrvHandleCPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureSrvHandleGPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
 //   textureSrvHandleCPU2.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureSrvHandleGPU2.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
 //   textureSrvHandleCPU3.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureSrvHandleGPU3.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//// SRVの設定
	//dxCommon->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
	//dxCommon->GetDevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);
	//dxCommon->GetDevice()->CreateShaderResourceView(textureResource3.Get(), &srvDesc3, textureSrvHandleCPU3);

    std::string textureFilePath[2]{ "Resources/monsterBall.png" ,"Resources/uvChecker.png" };
    std::vector<Sprite*>sprites;
	for (uint32_t i = 0; i < 10; ++i) {
		Sprite* sprite = new Sprite();
		sprite->Initialize(spriteCommon, textureFilePath[1]);
		sprites.push_back(sprite);
	}
    int i = 0;
	for (Sprite* sprite : sprites) {
		Vector2 position = sprite->GetPosition();
		Vector2 size = sprite->GetSize();

		position.x = 200.0f * i;
		position.y = 200.0f;
		size = Vector2(100, 100);

		sprite->SetPosition(position);
		sprite->SetSize(size);
		sprite->SetAnchorPoint(Vector2{ 0.0f,0.0f });
		sprite->SetIsFlipY(0);
		sprite->SetTextureLeftTop(Vector2{ i * 64.0f,0.0f });
		sprite->SetTextureSize(Vector2{ 512.0f,512.0f });
		i++;
	}

	Vector2 rotation{ 0 };

    static ImVec4 ballColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    static ImVec4 spriteColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    static ImVec4 modelColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    static Vector3 translate = { 0.0f, 0.0f, 0.0f };
    static Vector3 rotate = { 0.0f, 0.0f, 0.0f };
    static Vector3 scale = { 0.5f, 0.5f, 0.5f };
    static Vector3 modelTranslate = { 0.0f, 0.0f, 0.0f };
    static Vector3 modelRotate = { 0.0f, 0.0f, 0.0f };
    static Vector3 modelScale = { 0.5f, 0.5f, 0.5f };

    bool useMonsterBall = true;

    bool enableLighting = true;
    bool enableLightingSprite = false;
    bool enableLightingModel = true;

    Transform uvTransformSprite{
        {1.0f,1.0f,1.0f},
        {0.0f,0.0f,0.0f},
        {0.0f,0.0f,0.0f}
    };
    Transform uvTransformModel{
    {1.0f,1.0f,1.0f},
    {0.0f,0.0f,0.0f},
    {0.0f,0.0f,0.0f}
    };

    bool showSphere = false;
bool showSprite = true;
bool showModel = false;


    MSG msg{};
    while (msg.message != WM_QUIT) {
        if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        else {
            // ゲーム処理

            Matrix4x4 cameraMatrix =  Math::MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);

            // ワールド、ビュー、プロジェクションマトリックスを計算して設定する
            transform.rotate = rotate;
            rotate.y += 0.01f;
            transform.scale = scale;
            transform.translate = translate;
            Matrix4x4 worldMatrix =  Math::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
            Matrix4x4 viewMatrix =  Math::Inverse(cameraMatrix);
            Matrix4x4 projectionMatrix =  Math::MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
            Matrix4x4 worldViewProjectionMatrix =  Math::Multiply(worldMatrix,  Math::Multiply(viewMatrix, projectionMatrix));
            wvpData->WVP = worldViewProjectionMatrix;
            wvpData->World = worldMatrix;



          for (Sprite* sprite : sprites) {
			sprite->Update();
		}


            ////uvTransformMatrix用の行列
            //Matrix4x4 uvTransformMatrix =  Math::MakeScaleMatrix(uvTransformSprite.scale);
            //uvTransformMatrix =  Math::Multiply(uvTransformMatrix,  Math::MakeRotateZMatrix(uvTransformSprite.rotate.z));
            //uvTransformMatrix =  Math::Multiply(uvTransformMatrix,  Math::MakeTranslateMatrix(uvTransformSprite.translate));
            //materialDataSprite->uvTransform = uvTransformMatrix;
            
            //Model
            transformModel.rotate = modelRotate;
            transformModel.scale = modelScale;
            transformModel.translate = modelTranslate;
            Matrix4x4 worldMatrixModel =  Math::MakeAffineMatrix(transformModel.scale, transformModel.rotate, transformModel.translate);
            Matrix4x4 viewMatrixModel =  Math::Inverse(cameraMatrix);
            Matrix4x4 projectionMatrixModel =  Math::MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
            Matrix4x4 worldViewProjectionMatrixModel = Math:: Multiply(worldMatrixModel,  Math::Multiply(viewMatrixModel, projectionMatrixModel));
            modelWvpData->WVP = worldViewProjectionMatrixModel;
            modelWvpData->World = worldMatrixModel;
            
           dxCommon->Begin();
           //spriteCommon->CommonDraw();
           // // 3D球
           //   if (showSphere) {
           //      dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
           //     dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView);
           //     dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

           //     materialData->color.x = ballColor.x;
           //     materialData->color.y = ballColor.y;
           //     materialData->color.z = ballColor.z;
           //     materialData->color.w = ballColor.w;
           //     materialData->enableLighting = enableLighting;
           //     dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
           //     dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
           //     dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

           //     dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);
           //     dxCommon->GetCommandList()->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
           // }

           // // 
           spriteCommon->CommonDraw();

            // 2DSprite
            if (showSprite) {
               
                for (Sprite* sprite : sprites) {
                    sprite->Draw();
                }
                /*dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
                dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);
                dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                materialDataSprite->color.x = spriteColor.x;
                materialDataSprite->color.y = spriteColor.y;
                materialDataSprite->color.z = spriteColor.z;
                materialDataSprite->color.w = spriteColor.w;
                materialDataSprite->enableLighting = enableLightingSprite;
                dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
                dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
                dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResourceSprite->GetGPUVirtualAddress());
                dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
                dxCommon->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);*/
            }

            ////
            //spriteCommon->CommonDraw();

            ////model
            //if (showModel) {
            //    dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewModel);
            //    dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            //    materialDataModel->color.x = modelColor.x;
            //    materialDataModel->color.y = modelColor.y;
            //    materialDataModel->color.z = modelColor.z;
            //    materialDataModel->color.w = modelColor.w;
            //    materialDataModel->enableLighting = enableLightingModel;
            //    dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceModel->GetGPUVirtualAddress());
            //    dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, modelWvpResource->GetGPUVirtualAddress());
            //    dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResourceModel->GetGPUVirtualAddress());
            //    dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU3);
            //    dxCommon->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
            //}



            // ImGuiの新しいフレームを開始する
            ImGui_ImplDX12_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            // ImGuiウィンドウの設定
            ImGui::Begin("Window");

            ImGui::Text("Camera");
            ImGui::DragFloat3("Camera Position", &cameraTransform.translate.x, 0.1f);
            ImGui::DragFloat3("Camera Rotation", &cameraTransform.rotate.x, 0.1f);

            ImGui::Text("Draw Mode");
            if (ImGui::Button("Sphere")) {
                showSphere = !showSphere;
            }
            if (ImGui::Button("Sprite")) {
                showSprite = !showSprite;
            }
            if (ImGui::Button("Model")) {
                showModel = !showModel;
            }

            if (showSphere) {
                ImGui::Text("Ball");
                ImGui::ColorEdit4("Color", &ballColor.x);
                ImGui::DragFloat3("Translate", &translate.x, 0.1f);
                ImGui::DragFloat3("Rotate", &rotate.x, 0.1f);
                ImGui::DragFloat3("Scale", &scale.x, 0.1f);
                ImGui::Checkbox("useMonsterBall", &useMonsterBall);
                ImGui::Checkbox("enableLighting", &enableLighting);
                ImGui::ColorEdit4("Light Color", &directionalLightData->color.x);
                ImGui::DragFloat3("Light Direction", &directionalLightData->direction.x, 0.1f);
                directionalLightData->direction =  Math::Normalize(directionalLightData->direction);
                ImGui::DragFloat("Light Intensity", &directionalLightData->intensity, 0.1f);
            }

            if (showSprite) {
                ImGui::Text("Sprite");
                ImGui::ColorEdit4("Sprite Color", &spriteColor.x);
                ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
                ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
                ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
            }

            if (showModel) {
                ImGui::Text("Model");
                ImGui::DragFloat3("Model Translate", &modelTranslate.x, 0.1f);
                ImGui::DragFloat3("Model Rotate", &modelRotate.x, 0.1f);
                ImGui::DragFloat3("Model Scale", &modelScale.x, 0.1f);
                ImGui::Checkbox("enableLighting", &enableLightingModel);
                ImGui::ColorEdit4("Light Color", &directionalLightDataModel->color.x);
                ImGui::DragFloat3("Light Direction", &directionalLightDataModel->direction.x, 0.1f);
                directionalLightDataModel->direction = Math::Normalize(directionalLightDataModel->direction);
                ImGui::DragFloat("Light Intensity", &directionalLightDataModel->intensity, 0.1f);
            }

            ImGui::End();
        // ImGuiの描画データをレンダリングする
        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());
         dxCommon->End();
    }
}

    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    delete input;
    winApp->Finalize();
    delete winApp;
    delete dxCommon;
    for (Sprite* sprite : sprites) {
		delete sprite;
	}

    delete spriteCommon;
	return 0;
}
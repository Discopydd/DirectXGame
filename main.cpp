#include<Windows.h>
#include <sstream>
#include<fstream>
#include<string>
#include<format>
#include <wrl.h>
#include<random>
#include <algorithm>
#include <list>

#include"Struct.h"
#include"MyMath.h"
#include"DebugReporter.h"
#include"Input.h"
#include"WinApp.h"
#include <numbers>
#include "Logger.h"
#include "DirectXCommon.h"
#include "D3DResourceLeakChecker.h"
#include "SpriteCommon.h"
#include "Sprite.h"


enum BlendMode {
    kBlendModeNone,
    kBlendModeNormal,
    kBlendModeAdd,
    kBlendModeSubtract,
    kBlendModeMultily,
    kBlendModeScreen,
    kCountOfBlendMode
};
std::random_device seedGenerator;
std::mt19937 randomEngine(seedGenerator());

Particle MakeNewParticle(std::mt19937& randomEngine,const Vector3&translate) {
    std::uniform_real_distribution<float> distribution(-0.5f, 0.5f);
    std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
    std::uniform_real_distribution<float> distTime(1.0f, 2.0f);

    Vector3 randomTranslate{ distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
    Particle particle;

    particle.transform.scale = { 0.25f, 0.5f, 0.5f };
    particle.transform.rotate = { 0.0f, 3.0f, 0.0f };
    particle.transform.translate = Add(translate , randomTranslate);
    particle.velocity = { 
        distribution(randomEngine), 
        distribution(randomEngine), 
        distribution(randomEngine) 
    };
    particle.color = {
        distColor(randomEngine),
        distColor(randomEngine),
        distColor(randomEngine),1.0f
    };
    particle.lifeTime = distTime(randomEngine);
    particle.currentTime = 0;
    return particle;
}
//Particleを斜出する
std::list<Particle>Emit(const Emitter& emitter, std::mt19937& randomEngine) {
	std::list<Particle>particles;
	for (uint32_t count = 0; count < emitter.count; ++count) {
		particles.push_back(MakeNewParticle(randomEngine,emitter.transform.translate));
	}
	return particles;
}



MateriaData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
    MateriaData materiaData;
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

    D3DResourceLeakChecker leakCheck;

    CoInitializeEx(0, COINIT_MULTITHREADED);

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
    spriteCommon->Initialize();


    Sprite* sprite = new Sprite();
    sprite->Initialize();


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

    const float kLonEvery = 2 * (float)Math::PI / (float)kSubdivision; // 经度分割1つの角度
    const float kLatEvery = (float)Math::PI / (float)kSubdivision; // 纬度分割1つの角度

    uint32_t vertexIndex = 0;
    for (uint32_t latIndex = 0; latIndex <= kSubdivision; ++latIndex) {
        float lat = -(float)Math::PI / 2.0f + kLatEvery * latIndex; // 現在の緯度
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

    /*Transform cameraTransform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -5.0f} };*/
    Transform cameraTransform = {
{1.0f, 1.0f, 1.0f},
{std::numbers::pi_v<float> / 3.0f, std::numbers::pi_v<float>, 0.0f},
{0.0f, 23.0f, 10.0f}
    };
#pragma endregion

#pragma region Sprite
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite = dxCommon->CreateBufferResource(sizeof(VertexData) * 4);
    D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
    vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
    vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 4;
    vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);
    //頂点リソースにデータを書き込む
    VertexData* vertexDataSprite = nullptr;
    // 書き込むためのアドレスを取得
    vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
    // 左下
    vertexDataSprite[0].position = { 0.0f, 360.0f, 0.0f, 1.0f };
    vertexDataSprite[0].texcoord = { 0.0f, 1.0f };
    vertexDataSprite[0].normal = { 0.0f,0.0f,-1.0f };
    // 左上
    vertexDataSprite[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };
    vertexDataSprite[1].texcoord = { 0.0f, 0.0f };
    vertexDataSprite[1].normal = { 0.0f,0.0f,-1.0f };
    // 右下
    vertexDataSprite[2].position = { 640.0f, 360.0f, 0.0f, 1.0f };
    vertexDataSprite[2].texcoord = { 1.0f, 1.0f };
    vertexDataSprite[2].normal = { 0.0f,0.0f,-1.0f };
    // 右上
    vertexDataSprite[3].position = { 640.0f, 0.0f, 0.0f, 1.0f };
    vertexDataSprite[3].texcoord = { 1.0f, 0.0f };
    vertexDataSprite[3].normal = { 0.0f,0.0f,-1.0f };

    Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite = dxCommon->CreateBufferResource(sizeof(uint32_t) * 6);
    D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
    indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
    indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
    indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;
    //頂点リソースにデータを書き込む
    uint32_t* indexDataSprite = nullptr;
    // 書き込むためのアドレスを取得
    indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));

    indexDataSprite[0] = 0;
    indexDataSprite[1] = 1;
    indexDataSprite[2] = 2;
    indexDataSprite[3] = 1;
    indexDataSprite[4] = 3;
    indexDataSprite[5] = 2;


    // CPUで動かす用のTransformを作る
    Transform transformSprite = {};
    transformSprite.scale = { 1.0f, 1.0f, 1.0f };
    transformSprite.rotate = { 0.0f, 0.0f, 0.0f };
    transformSprite.translate = { 0.0f, 0.0f, 0.0f };


#pragma endregion

#pragma region モデル
    // モデル読み込み
    ModelData modelData;
    modelData.vertices.push_back({ .position = {1.0f,  1.0f,  0.0f, 1.0f}, .texcoord = {0.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 左上
    modelData.vertices.push_back({ .position = {-1.0f,  1.0f,  0.0f, 1.0f}, .texcoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 右上
    modelData.vertices.push_back({ .position = {1.0f, -1.0f,  0.0f, 1.0f}, .texcoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 左下
    modelData.vertices.push_back({ .position = {1.0f, -1.0f,  0.0f, 1.0f}, .texcoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 左下
    modelData.vertices.push_back({ .position = {-1.0f,  1.0f,  0.0f, 1.0f}, .texcoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 右上
    modelData.vertices.push_back({ .position = {-1.0f, -1.0f,  0.0f, 1.0f}, .texcoord = {1.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 右下
    modelData.material.textureFilePath = "./Resources/uvChecker.png";

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

#pragma region
    const uint32_t kNumMaxInstance = 100;  // 实例的数量
    // 
    Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource = dxCommon->CreateBufferResource(
        sizeof(ParticleForGPU) * kNumMaxInstance);

    // 获取写入数据的指针
    ParticleForGPU* instancingData = nullptr;
    instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));

    // 初始化 TransformationMatrix 数据
    for (uint32_t index = 0; index < kNumMaxInstance; ++index) {
        instancingData[index].WVP = MakeIdentity4x4();
        instancingData[index].World = MakeIdentity4x4();
        instancingData[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    }
#pragma endregion



    // Textureを読んで転送する
    DirectX::ScratchImage mipImages = dxCommon->LoadTexture("Resources/circle.png");
    const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
    Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = dxCommon->CreateTextureResource(metadata);
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = dxCommon->UploadTextureData(textureResource, mipImages);



    // metaDataを基にSRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
    srvDesc.Texture2D.MipLevels = static_cast<UINT>(metadata.mipLevels);


    D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc = {};
    instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
    instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    instancingSrvDesc.Buffer.FirstElement = 0;
    instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    instancingSrvDesc.Buffer.NumElements = kNumMaxInstance;
    instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);

    D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU = dxCommon->GetSRVCPUDescriptorHandle(4);
    D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU = dxCommon->GetSRVGPUDescriptorHandle(4);

    //SRVを作成する DescriptorHeapの場所を決める
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon->GetSRVCPUDescriptorHandle(1);
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = dxCommon->GetSRVGPUDescriptorHandle(1);


    textureSrvHandleCPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    textureSrvHandleGPU.ptr += dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    // SRVの設定
    dxCommon->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
    dxCommon->GetDevice()->CreateShaderResourceView(instancingResource.Get(), &instancingSrvDesc, instancingSrvHandleCPU);
#pragma endregion 


    static ImVec4 ballColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    static ImVec4 spriteColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    static ImVec4 modelColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    static Vector3 translate = { 0.0f, 0.0f, 0.0f };
    static Vector3 rotate = { 0.0f, 0.0f, 0.0f };
    static Vector3 scale = { 0.5f, 0.5f, 0.5f };
    static Vector3 translateSprite = { 0.0f, 0.0f, 0.0f };
    static Vector3 modelTranslate = { 0.0f, 0.0f, 0.0f };
    static Vector3 modelRotate = { 0.0f, 0.0f, 0.0f };
    static Vector3 modelScale = { 0.5f, 0.5f, 0.5f };


    bool useMonsterBall = true;

    bool enableLighting = true;
    bool enableLightingSprite = false;
    bool enableLightingModel = true;
    bool update = false;

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

    Emitter emitter{};
    emitter.count = 3;
    emitter.frequency = 0.5f;//0.5秒ごとに発生
    emitter.frequencyTime = 0.0f;//発生頻度用の時刻、0で初期化
    emitter.transform.translate = { 0.0f,0.0f,0.0f };
    emitter.transform.rotate = { 0.0f,0.0f,0.0f };
    emitter.transform.scale = { 1.0f,1.0f,1.0f };

    //Field
    AccelerationField accelerationField;
    accelerationField.acceleration = { 15.0f,0.0f,0.0f };
    accelerationField.area.min = { -1.0f,-1.0f,-1.0f };
    accelerationField.area.max = { 1.0f,1.0f,1.0f };



    std::list<Particle>particles;
    particles.push_back(MakeNewParticle(randomEngine, emitter.transform.translate));
    const float kDeltaTime = 1.0f / 60.0f;

    //ランダム


    bool showSphere = false;
    bool showSprite = false;
    bool showModel = true;
    bool useBillboard = true;



    while (true)
    {
        if (winApp->ProcessMessage()) {
            break;
        }
        else {
            input->Update();
            if (input->TriggerKey(DIK_0)) {
                OutputDebugStringA("Hit 0\n");
            }
            // ImGuiの新しいフレームを開始する
            ImGui_ImplDX12_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            // ImGuiウィンドウの設定
            ImGui::Begin("Window");

            ImGui::Text("Camera");
            ImGui::DragFloat3("Camera Position", &cameraTransform.translate.x, 0.1f);
            ImGui::DragFloat3("Camera Rotation", &cameraTransform.rotate.x, 0.1f);
            ImGui::DragFloat3("Camera Translate", &cameraTransform.translate.x, 0.1f);
            ImGui::Text("Draw Mode");
            //if (ImGui::Button("Sphere")) {
            //    showSphere = !showSphere;
            //}
            //if (ImGui::Button("Sprite")) {
            //    showSprite = !showSprite;
            //}

            if (ImGui::Button("Model")) {
                showModel = !showModel;
            }

            //if (showSphere) {
            //    ImGui::Text("Ball");
            //    ImGui::ColorEdit4("Color", &ballColor.x);
            //    ImGui::DragFloat3("Translate", &translate.x, 0.1f);
            //    ImGui::DragFloat3("Rotate", &rotate.x, 0.1f);
            //    ImGui::DragFloat3("Scale", &scale.x, 0.1f);
            //    ImGui::Checkbox("useMonsterBall", &useMonsterBall);
            //    ImGui::Checkbox("enableLighting", &enableLighting);
            //    ImGui::ColorEdit4("Light Color", &directionalLightData->color.x);
            //    ImGui::DragFloat3("Light Direction", &directionalLightData->direction.x, 0.1f);
            //    directionalLightData->direction = Normalize(directionalLightData->direction);
            //    ImGui::DragFloat("Light Intensity", &directionalLightData->intensity, 0.1f);
            //}

            //if (showSprite) {
            //    ImGui::Text("Sprite");
            //    ImGui::ColorEdit4("Sprite Color", &spriteColor.x);
            //    ImGui::DragFloat3("TranslateSprite", &translateSprite.x, 1.0f);
            //    ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
            //    ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
            //    ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
            //}

            if (showModel) {
                ImGui::Text("Model");
                ImGui::ColorEdit4("Color", &modelColor.x);
                ImGui::DragFloat3("Model Translate", &modelTranslate.x, 0.1f);
                ImGui::DragFloat3("Model Rotate", &modelRotate.x, 0.1f);
                ImGui::DragFloat3("Model Scale", &modelScale.x, 0.1f);
                ImGui::Checkbox("enableLighting", &enableLightingModel);
                ImGui::ColorEdit4("Light Color", &directionalLightDataModel->color.x);
                ImGui::DragFloat3("Light Direction", &directionalLightDataModel->direction.x, 0.1f);
                directionalLightDataModel->direction = Normalize(directionalLightDataModel->direction);
                ImGui::DragFloat("Light Intensity", &directionalLightDataModel->intensity, 0.1f);
                ImGui::Checkbox("update", &update);
                ImGui::Checkbox("Use Billboard", &useBillboard);
                ImGui::DragFloat3("EmitterTranslate", &emitter.transform.translate.x, 0.01f, -100.0f, 100.0f);
            }

            ImGui::End();
            // ImGuiの描画データをレンダリングする
            ImGui::Render();


            // ゲーム処理
            Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);

            // ワールド、ビュー、プロジェクションマトリックスを計算して設定する
            transform.rotate = rotate;
            rotate.y += 0.01f;
            transform.scale = scale;
            transform.translate = translate;
            Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
            Matrix4x4 viewMatrix = Inverse(cameraMatrix);
            Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
            Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
            Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
            wvpData->WVP = worldViewProjectionMatrix;
            wvpData->World = worldMatrix;
            // Sprite用のWorldViewProjectionMatrixを作る
            transformSprite.translate = translateSprite;
            Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
            Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
            Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, float(WinApp::kClientWidth), 0.0f, float(WinApp::kClientHeight), 0.0f, 100.0f);
            Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
            transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;
            transformationMatrixDataSprite->World = worldMatrixSprite;
            //uvTransformMatrix用の行列
            Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
            uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
            uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));
            materialDataSprite->uvTransform = uvTransformMatrix;
            //Model
            transformModel.rotate = modelRotate;
            transformModel.scale = modelScale;
            transformModel.translate = modelTranslate;
            Matrix4x4 worldMatrixModel = MakeAffineMatrix(transformModel.scale, transformModel.rotate, transformModel.translate);
            Matrix4x4 viewMatrixModel = Inverse(cameraMatrix);
            Matrix4x4 projectionMatrixModel = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
            Matrix4x4 worldViewProjectionMatrixModel = Multiply(worldMatrixModel, Multiply(viewMatrixModel, projectionMatrixModel));
            modelWvpData->WVP = worldViewProjectionMatrixModel;
            modelWvpData->World = worldMatrixModel;


            Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
            Matrix4x4 billboardMatrix;
            //チェック入れたらbillboard使う
            if (useBillboard) {
                billboardMatrix = Multiply(backToFrontMatrix, viewMatrix);
                billboardMatrix.m[3][0] = 0.0f;
                billboardMatrix.m[3][1] = 0.0f;
                billboardMatrix.m[3][2] = 0.0f;
            }
            //入れない場合単位行列
            else if (!useBillboard) {
                billboardMatrix = MakeIdentity4x4();
            }

            uint32_t numInstance = 0;
            //Emitterの更新

            emitter.frequencyTime += kDeltaTime;

            if (emitter.frequency <= emitter.frequencyTime) {//頻度より大きいなら発生
                particles.splice(particles.end(), Emit(emitter, randomEngine));
                emitter.frequencyTime -= emitter.frequency;//余計に過ぎた時間を加味して頻度計算
            }

            //Particleの更新
            for (std::list<Particle>::iterator particleIterator = particles.begin();
                particleIterator != particles.end();) {

                float alpha = 1.0f - ((*particleIterator).currentTime / (*particleIterator).lifeTime);
                if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
                    //生存時間が過ぎてparticleはlistから消す
                    particleIterator = particles.erase(particleIterator);
                    continue;
                }
                if (numInstance < kNumMaxInstance) {
                    Matrix4x4 scaleMatrix = MakeScaleMatrix((*particleIterator).transform.scale);
                    Matrix4x4 translateMatrix = MakeTranslateMatrix((*particleIterator).transform.translate);
                    Matrix4x4 worldMatrix = Multiply(scaleMatrix, Multiply(billboardMatrix, translateMatrix));
                    Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
                    instancingData[numInstance].WVP = worldViewProjectionMatrix;
                    instancingData[numInstance].World = worldMatrix;
                    instancingData[numInstance].color = (*particleIterator).color;
                    if (update) {
                        if (IsCollision(accelerationField.area, (*particleIterator).transform.translate)) {
                            (*particleIterator).velocity.x += accelerationField.acceleration.x * kDeltaTime;
                            (*particleIterator).velocity.y += accelerationField.acceleration.y * kDeltaTime;
                            (*particleIterator).velocity.z += accelerationField.acceleration.z * kDeltaTime;
                        }
                    }
                    (*particleIterator).transform.translate.x += (*particleIterator).velocity.x * kDeltaTime;
					(*particleIterator).transform.translate.y += (*particleIterator).velocity.y * kDeltaTime;
					(*particleIterator).transform.translate.z += (*particleIterator).velocity.z * kDeltaTime;
                    instancingData[numInstance].WVP = worldViewProjectionMatrix;
                    instancingData[numInstance].World = worldMatrix;
                    instancingData[numInstance].color.w = alpha;
                    ++numInstance;

                }
                ++particleIterator;
            }



            dxCommon->Begin();


            dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
            dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());
            ID3D12DescriptorHeap* descriptorHeaps[] = { dxCommon->GetSrvDescriptorHeap() };
            //dxCommon->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
            //        // 3D球
            //        if (showSphere) {
            //            dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
            //            dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView);
            //            dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            //
            //            materialData->color.x = ballColor.x;
            //            materialData->color.y = ballColor.y;
            //            materialData->color.z = ballColor.z;
            //            materialData->color.w = ballColor.w;
            //            materialData->enableLighting = enableLighting;
            //            dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
            //            dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
            //            dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
            //
            //            dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);
            //            dxCommon->GetCommandList()->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
            //        }
            //
            //dxCommon->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
            //        // 2DSprite
            //        if (showSprite) {
            //            dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
            //            dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);
            //            dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            //
            //            materialDataSprite->color.x = spriteColor.x;
            //            materialDataSprite->color.y = spriteColor.y;
            //            materialDataSprite->color.z = spriteColor.z;
            //            materialDataSprite->color.w = spriteColor.w;
            //            materialDataSprite->enableLighting = enableLightingSprite;
            //            dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
            //            dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
            //            dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResourceSprite->GetGPUVirtualAddress());
            //            dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
            //            dxCommon->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
            //        }

            dxCommon->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
            //model
            if (showModel) {
                dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewModel);
                dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                materialDataModel->color.x = modelColor.x;
                materialDataModel->color.y = modelColor.y;
                materialDataModel->color.z = modelColor.z;
                materialDataModel->color.w = modelColor.w;
                materialDataModel->enableLighting = enableLightingModel;

                dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceModel->GetGPUVirtualAddress());
                dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceModel->GetGPUVirtualAddress());
                dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
                dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
                dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(4, instancingSrvHandleGPU);
                //描画！
                dxCommon->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), numInstance, 0, 0);
            }
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
    delete sprite;
    delete spriteCommon;
    return 0;
}
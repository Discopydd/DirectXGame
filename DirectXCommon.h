#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include <wrl.h>
#include<cassert>
#include"WinApp.h"
#include <array>
#include <vector>
#include <dxcapi.h>
#include <chrono>
#include <thread>
#pragma comment(lib, "dxcompiler.lib")
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
class DirectXCommon
{
public:
	//初期化
	void Initialize(WinApp* windowsAPI);
	//描画前処理
	void Begin();
	//描画後処理
	void End();
	//テクスチャデータの転送
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);
	//デスクリプタヒープを生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisivle);
	//SRVの指定番号のCPUデスクリプタハンドル取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);
	//SRVの指定番号のGPUデスクリプタハンドル取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);
	//CPUデスクリプタハンドル取得関数
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>
		& descriptorHeap, uint32_t descriptorSize, uint32_t index);
	//GPUデスクリプタハンドル取得関数
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>
		& descriptorHeap, uint32_t descriptorSize, uint32_t index);
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height);
    //シェーダーのコンパイル
	Microsoft::WRL::ComPtr<IDxcBlob>CompileShader(const std::wstring& filePath, const wchar_t* profile);
	//バッファリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource>CreateBufferResource(size_t sizeInBytes);
	//テクスチャリソースの生成
	Microsoft::WRL::ComPtr<ID3D12Resource>CreateTextureResource( const DirectX::TexMetadata& metadata);
	//テクスチャファイル読み込み
	static DirectX::ScratchImage LoadTexture(const std::string& filePath);
	//デバイスの取得
	Microsoft::WRL::ComPtr<ID3D12Device>GetDevice() { return device; }
	//コマンドリストの取得
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>GetCommandList(){ return commandList; }
private:
	//デバイスの初期化
	void DeviceInitialize();
	//コマンド関連の初期化
	void CommandInitialize();
	//スワップチェーンの生成
	void SwapchainGenerate();
	//深度バッファの生成
	void DepthbufferGenerate();
	//各種デスクリプタヒープの生成
	void DescriptorheapGenerate();
	//レンダーターゲットビューの初期化
	void RendertargetviewInitialize();
	//深度ステンシルビューの初期化
	void DepthstencilviewInitialize();
	//フェンスの初期化
	void FenceInitialize();
	//ビューポート矩形の初期化
	void ViewportInitialize();
	//シザリング矩形の初期化
	void ScissorrectInitialize();
	//DXCコンパイラの生成
	void DXCcompilerInitialize();
	//Imguiの初期化
	void ImguiInitialize();
	//FPS固定初期化
	void InitializeFixFPS();
	//FPS固定更新
	void UpdateFixFPS();
	//DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device>device;
	//DXGIファクトリ
	Microsoft::WRL::ComPtr<IDXGIFactory7>dxgiFactory;
	//コマンドアロケータ
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	//コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	//コマンドキュー
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	//IDXGISwapChain4
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
	//Resource
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	//DescriptorSizeSRVを取得
	uint32_t descriptorSizeSRV;
	//DescriptorSizeRTVを取得
	uint32_t descriptorSizeRTV;
	//DescriptorSizeDSVを取得
	uint32_t descriptorSizeDSV;
	//RTV用のDescriptorSize
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	//SRV用のDescriptorSize
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
	//DSV用のDescriptorSize
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;
	///深度ステンシルリソース
	Microsoft::WRL::ComPtr <ID3D12Resource> depthStencilResource;
	//フェンス
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	//DXCユーティリティ
	Microsoft::WRL::ComPtr<IDxcUtils>dxcUtils;
	//DXCコンパイラ 
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler;
	//デフォルトインクルードハンドラ
	Microsoft::WRL::ComPtr<IDxcIncludeHandler>includeHandler;
	//頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	// ビューポート
	D3D12_VIEWPORT viewport{};
	// シザー矩形
	D3D12_RECT scissorRect{};
	//WindowsAPI
	WinApp* winApp = nullptr;
	//スワップチェインリソース
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources;
	//
	HANDLE fenceEvent;
	//フェンス値
	UINT64 fenceValue = 0;
	//スワップチェーンを生成
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//RTVハンドルの要素数を2個に変更する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	HRESULT hr;
	//記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_;


};

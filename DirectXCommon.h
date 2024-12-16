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
class DirectXCommon {
public: // メンバ関数
	void DeviceInitialize();
	void CommandInitialize();
	void SwapChainInitialize();
	void DepthBufferInitialize();
	void DescriptorHeapInitialize();
	void RTVInitialize();
	void DSVInitialize();
	void FenceInitialize();
	void ViewportInitialize();
	void ScissorInitialize();
	void DxcCompilerInitialize();
	void ImguiInitialize();
	public:
	//初期化
	void Initialize(WinApp* winApp);
	//描画前処理
	void Begin();
	//描画後処理
	void End();


	//SRVの指定番号のCPUデスクリプタハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);

	//SRVの指定番号のGPUデスクリプタハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);

	//RTVの指定番号のCPUデスクリプタハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUDescriptorHandle(uint32_t index);

	//RTVの指定番号のGPUデスクリプタハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetRTVGPUDescriptorHandle(uint32_t index);

	//DSVの指定番号のCPUデスクリプタハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCPUDescriptorHandle(uint32_t index);

	//DSVの指定番号のGPUデスクリプタハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetDSVGPUDescriptorHandle(uint32_t index);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap,
		uint32_t descriptorSize, uint32_t index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap,
		uint32_t descriptorSize, uint32_t index);

	ID3D12Device* GetDevice() const { return device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList()const { return commandList.Get(); }

	//CompileShader関数の作成
	IDxcBlob* CompileShader(
		//ComilerするSahaderファイルへのパス
		const std::wstring& filePath,
		//compilerに使用するProfile
		const wchar_t* profile);


	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

	[[nodiscard]]
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource>texture, const DirectX::ScratchImage& mipImages);

	//テクスチャファイルの読み込み
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	ID3D12DescriptorHeap* GetSrvDescriptorHeap() const { return srvDescriptorHeap.Get(); }

private: // メンバ変数
	// ウィンドウズアプリケーション管理
	WinApp* winApp_ = nullptr;
	HRESULT hr;


	// Direct3D関連
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2>swapChainResources;

	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource;

	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStarHandle;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 2> rtvHandles;


	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	HANDLE fenceEvent;
	uint64_t fenceValue = 0;

	//ビューポート
	D3D12_VIEWPORT viewport{};

	//シザー矩形
	D3D12_RECT scissorRect{};

	//DXC
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	IDxcIncludeHandler* includeHandler = nullptr;

	//barrier
	D3D12_RESOURCE_BARRIER barrier{};

	std::chrono::steady_clock::time_point reference_;

private: // メンバ関数
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>
		CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType,
			UINT numDescriptrs, bool shaderVisible);

	//FPS固定初期化
	void InitializeFixFPS();
	//FPS固定更新
	void UpdateFixFPS();
};
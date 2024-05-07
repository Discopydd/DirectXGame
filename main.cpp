#include<Windows.h>
#include<cstdint>
#include<string>
#include<format>

#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")


LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	
	switch (msg) {

	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void Log(const std::string & message){
		OutputDebugStringA(message.c_str());
	}
std::wstring ConvertString(const std::string& str) {
    if (str.empty()) {
        return std::wstring();
    }

    auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
    if (sizeNeeded == 0) {
        return std::wstring();
    }
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
    return result;
}

std::string ConvertString(const std::wstring& str) {
    if (str.empty()) {
        return std::string();
    }

    auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
    if (sizeNeeded == 0) {
        return std::string();
    }
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
    return result;
}
int WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int){
	
#pragma region Windowの生成

	WNDCLASS wc{};

	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = L"CG2WindowClass";
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	RegisterClass(&wc);

	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;

	RECT wrc = { 0,0,kClientWidth,kClientHeight };

	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	HWND hwnd = CreateWindow(
		wc.lpszClassName,
		L"CG2",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);

	ShowWindow(hwnd, SW_SHOW);

#pragma endregion

#ifdef _DEBUG
ID3D12Debug1* debugController = nullptr;
if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
	//デバッグレイヤーを有効化する
	debugController->EnableDebugLayer();
	//さらに6GPUでもチェックを行うようにする
	debugController->SetEnableGPUBasedValidation(TRUE);
}
#endif // DEBUG


#pragma region DXGIFactoryの生成

	IDXGIFactory7* dxgiFactory = nullptr;

	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(hr));
	#pragma endregion

#pragma region GPUを決定する
		//使用するアダプタ用の変数。初にnullptrを入れておく
	IDXGIAdapter4* useAdapter = nullptr;

	//良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; ++i) {
		//アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr)); // 取得できないのは一大事
		//ソフトウエアアダプタでなければ採用！
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {//採用したアダプタの情報をログに出力。wstringの方なので注意
			Log(ConvertString(std::format(L"Use Adapater:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;//ソフトウエアアダプタの場合は見なかったことにする
	}
    //適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);
	#pragma endregion

#pragma region D3D12Deviceの生成
	ID3D12Device* device = nullptr;
	//機能レ7レとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
	D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLeveLStrings[] = { "12.2","12.1","12.0" };
    //高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		//採用したアタプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter,
			featureLevels[i], IID_PPV_ARGS(&device));
		//指定した機能レ7レでデバイスが生成できたかを確認
		if (SUCCEEDED(hr)) {
			//生成できたのでログ出力を行ってループを扱ける
			Log(std::format("FeatureLevel : {}\n",
				featureLeveLStrings[i]));
			break;
		}
	}
//デバイスの生成がうまくいかなかったので起動できない
assert(device != nullptr);
Log("CompLete create D3D12Device!!!\n");//初期化完了のログをだす
#pragma endregion

#ifdef _DEBUG

ID3D12InfoQueue* infoQueue = nullptr;

if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {

	//ヤバイエラー時に止まる
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
	//エラー時に止まる
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	//警告時に止まる
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
	//抑制するメッセージのID

	D3D12_MESSAGE_ID denyIds[] = {

		//windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
		//https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11 
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
	};

    //抑制するレベル
	D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
	D3D12_INFO_QUEUE_FILTER filter{};
	filter.DenyList.NumIDs = _countof(denyIds);
	filter.DenyList.pIDList = denyIds;
	filter.DenyList.NumSeverities = _countof(severities); filter.DenyList.pSeverityList = severities;

   //指定したメッセージの表示を抑制する
	infoQueue->PushStorageFilter(&filter);
	//解放
	infoQueue->Release();
}
#endif


#pragma region ComandQueueを生成する
ID3D12CommandQueue* commandQueue = nullptr;

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));

	assert(SUCCEEDED(hr));
	#pragma endregion

#pragma region ComandListを生成する
	ID3D12CommandAllocator* commandAllocator = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(hr));

	ID3D12GraphicsCommandList* commandList = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr,
		IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(hr));
	#pragma endregion

#pragma region SwapChainを生成する
	IDXGISwapChain4* swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = kClientWidth;
	swapChainDesc.Height = kClientHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));
	assert(SUCCEEDED(hr));
#pragma endregion

#pragma region DesciptorHeapを生成する
	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorHeapDesc.NumDescriptors = 2;
	hr = device->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap));
	assert(SUCCEEDED(hr));
#pragma endregion

#pragma region SwapChainからResourceを引っ張ってくる
	ID3D12Resource* swapChainResources[2] = { nullptr };
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	assert(SUCCEEDED(hr));

	hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(hr));
#pragma endregion

#pragma region RTVを作る
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStarHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	rtvHandles[0] = rtvStarHandle;
	device->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);

	rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);
#pragma endregion

#pragma region FenceとEventを生成する
//初期値θでFenceを作る
ID3D12Fence* fence = nullptr; 
uint64_t fenceValue = 0;
hr= device->CreateFence(fenceValue,D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)); assert(SUCCEEDED(hr));

//FenceのSignalを待つためのイベントを作成する
HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
assert(fenceEvent != nullptr);
#pragma endregion

	MSG msg{};
	while (msg.message != WM_QUIT) {

		if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else {
			//ゲーム処理

		    //これから書き込むバックバッファのインデックスを取得
			UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

#pragma region TransitionBarrierを張るコード
			D3D12_RESOURCE_BARRIER barrier{};
//今回のバリアはTransition
barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; 
//Noneにしておく
barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
//バリアを張る対象のリソース。現在のバックバッファに対して行う
barrier.Transition.pResource = swapChainResources[backBufferIndex]; 
//遷移前（現在）のResourceState
barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
//遷移後のResourceState
barrier.Transition.StateAfter= D3D12_RESOURCE_STATE_RENDER_TARGET; 
//TransitionBarrierを張る
commandList->ResourceBarrier(1, &barrier);
	#pragma endregion

	        //描画先のRTVを設定する
			commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);
    //指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };//青っぽい色。RGBAの順
	commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);

//画面に描く処理はすべて終わり、画面に映すので、状態を遷移
//今回はRenderTargetからPresentにする
barrier.Transition.StateBefore=D3D12_RESOURCE_STATE_RENDER_TARGET; barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
//TransitionBarrierを張る
commandList->ResourceBarrier(1, &barrier); 
    //コマンドリストの内容を確定させる。
	hr = commandList->Close();
  assert(SUCCEEDED(hr));
 //GPWこコマンドリストの実行を行わせる
  ID3D12CommandList* commandLists[] = { commandList };
  commandQueue->ExecuteCommandLists(1, commandLists);
  //GPUとOSに画面の交換を行うよう通知する
  swapChain->Present(1, 0);
 //Fenceの値を更新
  fenceValue++;
//GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
  commandQueue->Signal(fence, fenceValue);
  //Fenceの値が指定したSignal値にたどり着いているか確認する
  //GetCompletedValueの初期値はFence作成時に渡した初期値 
  if (fence->GetCompletedValue() < fenceValue) {

	  //指定したSignalにたどりついていないので、たどり着くまで待つようにイベントを設定する
	  fence->SetEventOnCompletion(fenceValue, fenceEvent);
	 //イベント待つ
	  WaitForSingleObject(fenceEvent, INFINITE);
  }
//次のフレーム用のコマンドリストを準備
  hr = commandAllocator->Reset();
  assert(SUCCEEDED(hr));
  hr = commandList->Reset(commandAllocator, nullptr);
  assert(SUCCEEDED(hr));
		}
	}
	OutputDebugStringA("Hello,DirectX\n");

	return 0;
}
#include "DirectXSetter.h"
#include <cassert>
#include "Drawing/TextureManager.h"
#include "DescriptorHeap/DescriptorHeapManager.h"
#include "base/DescriptorHandle.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

uint32_t DirectXSetter::srvHandleNumber_ = 0;

DirectXSetter* DirectXSetter::GetInstance() {
	static DirectXSetter instance;
	return &instance;
}

void DirectXSetter::Initialize(WindowManager* winApp, int32_t backBufferWidth, int32_t backBufferHeight) {

	//nullptrチェック
	assert(winApp);
	assert(4 <= backBufferWidth && backBufferWidth <= 4096);
	assert(4 <= backBufferHeight && backBufferHeight <= 4096);

	windowManager_ = winApp;
	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;

	//FPS固定初期化
	InitializeFixFPS();

	//デバイス初期化
	InitializeDXGIDevice();

	//デスクリプタヒープマネージャー初期化
	DescriptorHeapManager::GetInstance()->Initialize(device_.Get());

	//コマンド関連初期化
	InitializeCommand();

	//スワップチェーン生成
	CreateSwapChain();

	//srvHeap生成
	CreateSrvHeap();

	//レンダーターゲット生成
	CreateRenderTargets();

	//深度バッファ生成
	CreateDepthBuffer();

	//フェンス生成
	CreateFence();

}

void DirectXSetter::RenderTexturePreDraw() {

	//レンダーターゲットビュー用ディスクリプタヒープハンドル取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = GetCPUDescriptorHandle(rtvHeap_.Get(), descriptorSizeRTV, 2);
	//深度ステンシルビュー用ディスクリプタヒープハンドル取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = GetCPUDescriptorHandle(dsvHeap_.Get(), descriptorSizeDSV, 0);
	//レンダーターゲットをセット
	commandList_->OMSetRenderTargets(1, &rtvH, false, &dsvH);
	//指定した色で画面全体をクリアする
	float clearColor[] = { 0.05f,0.1f,0.5f,1.0f }; //青色
	commandList_->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	//指定した深度で画面全体をクリアする
	commandList_->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	//デスクリプタヒープセット
	ID3D12DescriptorHeap* descriptorHeaps[] = { TextureManager::GetInstance()->GetSRVDescHeap() };
	commandList_->SetDescriptorHeaps(1, descriptorHeaps);

	//ビューポート
	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = float(backBufferWidth_);
	viewport.Height = float(backBufferHeight_);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	commandList_->RSSetViewports(1, &viewport);

	//シザー矩形
	D3D12_RECT scissorRect{};
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = backBufferWidth_;
	scissorRect.top = 0;
	scissorRect.bottom = backBufferHeight_;
	commandList_->RSSetScissorRects(1, &scissorRect);

	

}

void DirectXSetter::RenderTexturePostDraw() {

}

void DirectXSetter::PreDraw() {

	//バックバッファの番号取得
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = backBuffers_[bbIndex].Get();
	//遷移前(現在)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);

	//レンダーターゲットビュー用ディスクリプタヒープハンドル取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = GetCPUDescriptorHandle(rtvHeap_.Get(), descriptorSizeRTV, bbIndex);
	//深度ステンシルビュー用ディスクリプタヒープハンドル取得
	/*D3D12_CPU_DESCRIPTOR_HANDLE dsvH = GetCPUDescriptorHandle(dsvHeap_, descriptorSizeDSV, 0);*/
	//レンダーターゲットをセット
	commandList_->OMSetRenderTargets(1, &rtvH, false, nullptr);
	//指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f }; //青っぽい色
	commandList_->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	//指定した深度で画面全体をクリアする
	/*commandList_->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);*/
	//デスクリプタヒープセット
	ID3D12DescriptorHeap* descriptorHeaps[] = { TextureManager::GetInstance()->GetSRVDescHeap() };
	commandList_->SetDescriptorHeaps(1, descriptorHeaps);

	//ビューポート
	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = float(backBufferWidth_);
	viewport.Height = float(backBufferHeight_);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	commandList_->RSSetViewports(1, &viewport);

	//シザー矩形
	D3D12_RECT scissorRect{};
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = backBufferWidth_;
	scissorRect.top = 0;
	scissorRect.bottom = backBufferHeight_;
	commandList_->RSSetScissorRects(1, &scissorRect);

}

void DirectXSetter::PostDraw() {

	HRESULT hr;

	//バックバッファの番号取得
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = backBuffers_[bbIndex].Get();
	//遷移前(現在)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);

	//コマンドリストの内容を確定させる。全てのコマンドを積んでからCloseすること
	hr = commandList_->Close();
	assert(SUCCEEDED(hr));

	//GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { commandList_.Get()};
	commandQueue_->ExecuteCommandLists(1, commandLists);
	//GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present(1, 0);
	//Fenceの値を更新
	fenceVal_++;
	//GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceVal_);
	//Fenceの値が指定したSignal値にたどり着いているか確認する
	//GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceVal_) {
		//FenceのSignalを持つためのイベントを作成する
		HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(fenceEvent != nullptr);
		//指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceVal_, fenceEvent);
		//イベント待つ
		WaitForSingleObject(fenceEvent, INFINITE);
		CloseHandle(fenceEvent);
	}

	//FPS固定
	UpdateFixFPS();

	//次のフレーム用のコマンドリストを準備
	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));

}

void DirectXSetter::InitializeDXGIDevice() {

#ifdef  _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		//デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		//更にGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(FALSE);
	}
#endif //  _DEBUG

	//HRESULTはWindows系のエラーコードであり、
	//関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	//初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
	//どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));

	//使用するアダプタ用の変数。最初にnullptrを入れておく
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter = nullptr;
	//良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; i++) {
		//アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr)); //取得できないのは一大事
		//ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			break;
		}
		useAdapter = nullptr; //ソフトウェアアダプタの場合は見なかったことにする
	}
	//適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);

	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
	};
	
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); i++) {
		//採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		//指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr)) {
			break;
		}
	}
	//デバイスの生成が上手くいかなかったので起動できない
	assert(!!device_);
	assert(SUCCEEDED(hr));

#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//ヤバイエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		/*infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);*/

		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			//https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}
#endif // _DEBUG

	//DescriptorSizeを取得しておく
	descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorSizeRTV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeDSV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

}

void DirectXSetter::InitializeCommand() {

	HRESULT hr = S_FALSE;

	//コマンドアロケータを生成する
	hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	//コマンドアロケータの生成が上手くいかなかったので起動できない
	assert(SUCCEEDED(hr));

	//コマンドリストを生成する
	hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(),
		nullptr, IID_PPV_ARGS(&commandList_));
	//コマンドリストの生成が上手くいかなかったので起動できない
	assert(SUCCEEDED(hr));

	//コマンドキューを生成する
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	//コマンドキューの生成が上手くいかなかったので起動できない
	assert(SUCCEEDED(hr));

}

void DirectXSetter::CreateSwapChain() {

	HRESULT hr = S_FALSE;

	//スワップチェーンを生成する
	swapChainDesc.Width = backBufferWidth_; //画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Height = backBufferHeight_; //画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //色の形式
	swapChainDesc.SampleDesc.Count = 1; //マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //描画のターゲットとして利用する
	swapChainDesc.BufferCount = 2; //ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //モニタにうつしたら、中身を破棄
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), windowManager_->GetHwnd(), &swapChainDesc,
		nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));

}

void DirectXSetter::CreateSrvHeap() {

	srvHeap_ = DescriptorHeapManager::GetInstance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 256, true, "SRVHeap");

	srvHandleNumber_ = 0;

	srvHeap_->SetName(L"srvHeap");

}

void DirectXSetter::CreateRenderTargets() {

	HRESULT hr = S_FALSE;

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	hr = swapChain_->GetDesc(&swcDesc);
	assert(SUCCEEDED(hr));

	rtvHeap_ = DescriptorHeapManager::GetInstance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3, false, "RTVHeap");

	rtvHeap_->SetName(L"rtvHeap");

	backBuffers_.resize(swcDesc.BufferCount);

	//レンダーターゲットビューの設定
	D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
	renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (int i = 0; i < backBuffers_.size(); i++) {

		//スワップチェーンからバッファを取得
		hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&backBuffers_[i]));
		assert(SUCCEEDED(hr));

		//ディスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE handle = GetCPUDescriptorHandle(rtvHeap_.Get(), descriptorSizeRTV, i);
		//レンダーターゲットビューの生成
		device_->CreateRenderTargetView(backBuffers_[i].Get(), &renderTargetViewDesc, handle);

	}

	
}

void DirectXSetter::CreateDepthBuffer() {

	HRESULT hr = S_FALSE;

	depthStencil_.Create(device_.Get(), WindowManager::kWindowWidth, WindowManager::kWindowHeight);

	dsvHeap_ = DescriptorHeapManager::GetInstance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false, "DSVHeap");

	dsvHeap_->SetName(L"dsvHeap");

	//DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; //2dTexture
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	//DSVHeapの先頭にDSVを作る
	device_->CreateDepthStencilView(depthStencil_.Get(), &dsvDesc, dsvHeap_->GetCPUDescriptorHandleForHeapStart());

}

void DirectXSetter::CreateFence() {

	HRESULT hr = S_FALSE;

	hr = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

}

//FPS固定初期化
void DirectXSetter::InitializeFixFPS() {

	//現在時間を記録する
	reference_ = std::chrono::steady_clock::now();

}

//FPS固定更新
void DirectXSetter::UpdateFixFPS() {

	//1/60秒ぴったりの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
	//1/60秒よりわずかに短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	//現在時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	std::chrono::microseconds elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	//1/60秒よりわずかに短い時間が経っていない場合
	if (elapsed < kMinTime) {
		//1/60秒経過するまで微小なスリーブを繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			//1マイクロ秒スリーブ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}

	}
	//現在の時間を記録する
	reference_ = std::chrono::steady_clock::now();

}

void DirectXSetter::Finalize() {

}

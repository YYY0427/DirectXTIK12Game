#include "DirectX12Wrapper.h"
#include "Application.h"
#include <cassert>
#include <array>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

DirectX12Wrapper::DirectX12Wrapper()
{
}

DirectX12Wrapper::~DirectX12Wrapper()
{

}

bool DirectX12Wrapper::Init(Application* app)
{
	// プリプロセッサ(_DEBUG)
#ifdef _DEBUG
	// デバッグレイヤーを有効化
	// DirectX12はデフォルトだとGPU側でエラーが起きても情報がない
	// デバッグレイヤーをオンにすることで、GPU側でエラーが出たらログを出力するように設定する
	ComPtr<ID3D12Debug>	debugLayer;
	 HRESULT r = D3D12GetDebugInterface(IID_PPV_ARGS(debugLayer.ReleaseAndGetAddressOf()));
	debugLayer->EnableDebugLayer();		// デバッグレイヤーを有効にする
	debugLayer.Reset();					// デバッグレイヤーオブジェクトを破棄
#endif 

	// DirectXDeviceというのとDXGIという仕組みがあり、この2つが連携をとって動くが、別物
#ifdef _DEBUG
	constexpr UINT debugFlg = DXGI_CREATE_FACTORY_DEBUG;
#else
	constexpr UINT debugFlg = 0;
#endif

	// DXGI(DirectXGraphicsInterface)の初期化
	HRESULT result = CreateDXGIFactory2(debugFlg, 
		IID_PPV_ARGS(dxgi_.ReleaseAndGetAddressOf()));

	// resultはいろんな値が帰ってくる
	assert(SUCCEEDED(result));	// SUCCEEDで囲むと、この結果がOK
	if (FAILED(result))			// FAILEDで囲むと、この結果がOUT
	{
		return false;
	}

	std::vector<IDXGIAdapter*> adapters;
	IDXGIAdapter* tmpAdapter = nullptr;
	for (int i = 0; dxgi_->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		adapters.push_back(tmpAdapter);
	}
	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);
		std::wstring strDesc = adesc.Description;
		// PCのGPUがAMDの場合、選ばれない可能性がある
		if (strDesc.find(L"NVIDIA") != std::string::npos)
		{
			tmpAdapter = adpt;
			break;
		}
	}

	// 機能レベルを最新から4つ前くらいまで用意しておく
	std::array<D3D_FEATURE_LEVEL, 4> fLevels = 
	{
		D3D_FEATURE_LEVEL_12_1,	// 最新
		D3D_FEATURE_LEVEL_12_0,	// 1世代前
		D3D_FEATURE_LEVEL_11_1,	// 2世代前
		D3D_FEATURE_LEVEL_11_0,	// 3世代前
	};

	// Direct3Dの初期化
	D3D_FEATURE_LEVEL decidedFeatureLevel = D3D_FEATURE_LEVEL_12_1;
	for (auto flv : fLevels)
	{
		// ReleaseAndGetAddress0fは、dev_のポインタのポインタを表している
		// DirectXの基本オブジェクトとなるD3DDeviceオブジェクトを取得する
		result = D3D12CreateDevice(
			nullptr,	// nullptrを入れれば、自動的に
			flv,		// フィーチャレベル(DirectXのバージョン)
			IID_PPV_ARGS(dev_.ReleaseAndGetAddressOf())
		);
		if (SUCCEEDED(result))
		{
			decidedFeatureLevel = flv;
			break;
		}
	}
	assert(SUCCEEDED(result));
	if (FAILED(result))
	{
		return false;
	}

	// コマンドキューの作成
	// コマンドキューとは、コマンドリストをグラボに押し出して実行するもの
	D3D12_COMMAND_QUEUE_DESC cmdQueDesc = {};
	cmdQueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;			// フラグなし
	cmdQueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;	// 通常順
	cmdQueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;			// コマンドリストを普通に実行
	cmdQueDesc.NodeMask = 0;									// 基本的に0
	result = dev_->CreateCommandQueue(&cmdQueDesc, IID_PPV_ARGS(cmdQue_.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{
		return false;
	}
	
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = app->GetWindowWidth();				// ウィンドウ幅
	scDesc.Height = app->GetWindowHeight();				// ウィンドウ高さ
	scDesc.Stereo = false;								// VRの時以外false
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;		// アルファは特にいじらない
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			// RGBA 8bitの標準

	// アンチエイリアシングにかかわってる
	scDesc.SampleDesc.Count = 1;							// AAなし
	scDesc.SampleDesc.Quality = 0;							// AAクオリティ最低
	scDesc.Scaling = DXGI_SCALING_STRETCH;					// 画面のサイズを変えると、それに合わせる
	scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// ウィンドウと全画面を切り替え許容
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	scDesc.BufferCount = 2;									// 表画面と裏画面の二枚

	ComPtr<IDXGISwapChain1> swapChain;
	result = dxgi_->CreateSwapChainForHwnd(
		cmdQue_.Get(), 
		app->GetWindowHandle(),
		&scDesc,
		nullptr,
		nullptr,
		swapChain.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(result));
	result = swapChain.As(&swapChain_);
	assert(SUCCEEDED(result));

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NodeMask = 0;
	rtvHeapDesc.NumDescriptors = 2;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	// レンダーターゲットビューとして
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dev_->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(rtvHeaps_.ReleaseAndGetAddressOf()));

	CD3DX12_CPU_DESCRIPTOR_HANDLE descHandle(rtvHeaps_->GetCPUDescriptorHandleForHeapStart(), 0, 0);
	swapChain_->GetDesc1(&scDesc);
	std::array<ID3D12Resource*, 2> renderTargetResources;
	for (int i = 0; i < scDesc.BufferCount; ++i)
	{
		result = swapChain_->GetBuffer(i, IID_PPV_ARGS(&renderTargetResources[i]));
		dev_->CreateRenderTargetView(renderTargetResources[i], nullptr, descHandle);
		descHandle.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}


	assert(SUCCEEDED(result));

	return true;
}

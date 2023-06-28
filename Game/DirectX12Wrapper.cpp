#include "DirectX12Wrapper.h"
#include "Application.h"
#include <cassert>
#include <array>
#include <d3dcompiler.h>
#include <WICTextureLoader.h>
#include <algorithm>
#include <GLTFSDK/GLTF.h>	
#include <GLTFSDK/GLBResourceReader.h>
#include <GLTFSDK/Deserialize.h>
#include <filesystem>
#include <fstream>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

// GLBのためのストリームリーダー
class GLBStreamReader : public Microsoft::glTF::IStreamReader
{
public:
	GLBStreamReader(){};
	virtual std::shared_ptr<std::istream> GetInputStream(const std::string& strPath) const override
	{
		std::filesystem::path filepath(strPath);
		auto stream = std::make_shared<std::ifstream>(filepath, std::ios_base::binary);
		if (!stream || !(*stream))
		{
			assert(0);
			return nullptr;
		}
		return stream;
	}
};

using namespace DirectX;

std::vector<DirectX12Wrapper::Vertex> DirectX12Wrapper::CreateCubeVertices()const
{
	std::vector<Vertex> vertices(10);
	//手前(Nの字)
	//左下
	vertices[0] = Vertex(
		XMFLOAT3(-1, -1, -1.0f),// 左下
		XMFLOAT3(1.0f, 1.0f, 1.0f),// 緑
		XMFLOAT2(0.0f, 1.0f)//左下
	);
	//左上
	vertices[1] = Vertex(
		XMFLOAT3(-1, +1, -1.0f),// 左上
		XMFLOAT3(1.0f, 1.0f, 1.0f),//赤
		XMFLOAT2(0.0f, 0.0f)//左上
	);
	//右下
	vertices[2] = Vertex(
		XMFLOAT3(+1, -1, -1.0f),// 右下
		XMFLOAT3(1.0f, 1.0f, 1.0f),// 黄色
		XMFLOAT2(1.0f, 1.0f)//右下
	);
	//右上
	vertices[3] = Vertex(
		XMFLOAT3(+1, +1, -1.0f),// 右上
		XMFLOAT3(1.0f, 1.0f, 1.0f),//青
		XMFLOAT2(1.0f, 0.0f)//右上
	);

	//奥側
	//右下
	vertices[4] = Vertex(
		XMFLOAT3(+1, -1, +1.0f),// 右下
		XMFLOAT3(1.0f, 1.0f, 1.0f),//赤
		XMFLOAT2(0.0f, 1.0f)//UV左下
	);
	//右上
	vertices[5] = Vertex(
		XMFLOAT3(+1, +1, +1.0f),// 右上
		XMFLOAT3(1.0f, 1.0f, 1.0f),//青
		XMFLOAT2(0.0f, 0.0f)//UV左上
	);
	//左下
	vertices[6] = Vertex(
		XMFLOAT3(-1, -1, +1.0f),// 左下
		XMFLOAT3(1.0f, 1.0f, 1.0f),// 緑
		XMFLOAT2(1.0f, 1.0f)//UV右下
	);
	//左上
	vertices[7] = Vertex(
		XMFLOAT3(-1, +1, +1.0f),// 右下
		XMFLOAT3(1.0f, 1.0f, 1.0f),// 黄色
		XMFLOAT2(1.0f, 0.0f)//UV右上
	);
	vertices[8] = Vertex(
		XMFLOAT3(1, 1, 1.0f),// 左下
		XMFLOAT3(1.0f, 1.0f, 1.0f),//赤
		XMFLOAT2(1.0f, 1.0f)//左下
	);
	vertices[9] = Vertex(
		XMFLOAT3(-1, 1, 1.0f),// 左上
		XMFLOAT3(1.0f, 1.0f, 1.0f),//青
		XMFLOAT2(0.0f, 1.0f)//右上
	);

	return vertices;
}

std::vector<uint32_t> DirectX12Wrapper::CreateCubeIndices() const
{

	std::vector<uint32_t> indices(6 * 2 * 3);
	int idx = 0;
	auto vertSize = 8;
	// 面ごとにループ
	for (int i = 0; i < 4; i++) {	// 4面
		//左下
		indices[idx] = (0 + i * 2) % vertSize;
		indices[idx + 1] = (1 + i * 2) % vertSize;
		indices[idx + 2] = (2 + i * 2) % vertSize;
		idx += 3;

		//右上
		indices[idx] = (1 + i * 2) % vertSize;
		indices[idx + 1] = (3 + i * 2) % vertSize;
		indices[idx + 2] = (2 + i * 2) % vertSize;
		idx += 3;
	}
	//上ふた
	indices[idx] = 1;
	indices[idx + 1] = 9;
	indices[idx + 2] = 3;
	idx += 3;

	indices[idx] = 9;
	indices[idx + 1] = 8;
	indices[idx + 2] = 3;
	idx += 3;

	//下ふた
	indices[idx] = 2;
	indices[idx + 1] = 4;
	indices[idx + 2] = 6;
	idx += 3;

	indices[idx] = 2;
	indices[idx + 1] = 6;
	indices[idx + 2] = 0;
	idx += 3;


	return indices;
}

bool DirectX12Wrapper::CreateBufferForCube()
{
	auto vertices = CreateCubeVertices();
	auto indices = CreateCubeIndices();

	HRESULT result = S_OK;

	//頂点バッファの作成
	//用途はともかくバッファを確保する関数がcreateCommittedResourceです。
	//確保するのですが、ここで用途に応じて区帆の仕方を最適化仕様と
	//します。このため、やたら細かい指摘が必要になります
	result = CreateMappableOneDimensionalBuffer(sizeof(Vertex) * vertices.size(), vertexBuffer_);
	if (FAILED(result))
	{
		return false;
	}

	// GPUが利用できる「インデックスバッファ」の作成
	result = CreateMappableOneDimensionalBuffer(sizeof(uint32_t) * indices.size(), indexBuffer_);
	if (FAILED(result))
	{
		return false;
	}

	//GPU上のメモリをいじるためにMap関数を事項する
	//GPU上のメモリは弄れないんだけど、双子メモリを
	//CPUからGPUの内容を弄れるようにする
	Vertex* vertMap = nullptr;
	result = vertexBuffer_->Map(0, nullptr, (void**)&vertMap);
	if (!CheckHRESULT(result))
	{
		return false;
	}

	// 頂点データをGPU上のバッファ(CPU上の双子メモリ)にコピーする
	//memcpyみたいな関数ですが、これはSTLの一種で、バッファオーバーランも
	//検出してくれるバージョンなので、こちらを使う
	std::copy(vertices.begin(), vertices.end(), vertMap);
	vertexBuffer_->Unmap(0, nullptr);

	vbView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	//全体のサイズ
	vbView_.SizeInBytes = sizeof(Vertex) * vertices.size();
	//Strideは歩幅の意味です。つまり、次のデータまでの距離を示す。
	vbView_.StrideInBytes = sizeof(Vertex);

	uint32_t* indexMap = nullptr;
	result = indexBuffer_->Map(0, nullptr, (void**)&indexMap);
	if (!CheckHRESULT(result))
	{
		return false;
	}
	std::copy(indices.begin(), indices.end(), indexMap);
	indexBuffer_->Unmap(0, nullptr);

	ibView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	ibView_.SizeInBytes = sizeof(int) * indices.size();
	ibView_.Format = DXGI_FORMAT_R32_UINT;
	return true;
}

HRESULT DirectX12Wrapper::CreateMappableOneDimensionalBuffer(size_t bufferSize, ComPtr<ID3D12Resource>& res)
{
	HRESULT result = S_OK;

	//GPUが利用できる「頂点バッファ」を作ります
	auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	result = dev_->CreateCommittedResource(
		&heapProperties,//確保されるメモリの特質
		D3D12_HEAP_FLAG_NONE,//特殊なフラグがあるか？
		&resourceDesc,//どこに配置すべきか
		D3D12_RESOURCE_STATE_GENERIC_READ,//最初の用途
		nullptr,//クリアするためのバッファ
		IID_PPV_ARGS(res.ReleaseAndGetAddressOf())
	);
	CheckHRESULT(result);
	return result;
}


bool DirectX12Wrapper::CreateMainRenderTarget()
{
	HRESULT result = S_OK;
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NodeMask = 0;
	rtvHeapDesc.NumDescriptors = 2;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;// レンダーターゲットビューとして
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	result = dev_->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(rtvHeaps_.ReleaseAndGetAddressOf()));
	if (!CheckHRESULT(result))
	{
		return false;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE descHandle = { rtvHeaps_->GetCPUDescriptorHandleForHeapStart() };

	// すでに表画面と裏画面はSwapChain内にあるので、それを取得できるようにswapChainの情報を取得しておく
	DXGI_SWAP_CHAIN_DESC1 scDescForRTV = {};
	swapChain_->GetDesc1(&scDescForRTV);

	// スワップチェーンが持ってるバッファ取得用
	//std::array<ID3D12Resource*, 2> renderTargetResources;

	// スワップチェーン内のバッファ数分ループする
	for (int i = 0; i < scDescForRTV.BufferCount; ++i)
	{
		// スワップチェーンの特定番号のバッファを取得する
		result = swapChain_->GetBuffer(i, IID_PPV_ARGS(&rtvResources_[i]));
		dev_->CreateRenderTargetView(rtvResources_[i], nullptr, descHandle);
		descHandle.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	if (!CheckHRESULT(result))
	{
		return false;
	}
	return true;
}

bool DirectX12Wrapper::InitDirectXDevice()
{
	// デバッグ出力を有効にする
	ValidateDebugOutput();

	// DXGI(DirectXGraphicsInterface)の初期化
	HRESULT result = S_OK;
	result = InitDXGI();
	if (FAILED(result))
	{
		return false;
	}
	
	// 機能レベルを最新から4つ前くらいまで用意しておく
	std::array<D3D_FEATURE_LEVEL, 4> fLevels =
	{
		D3D_FEATURE_LEVEL_12_1,// 最新
		D3D_FEATURE_LEVEL_12_0,// 1世代前
		D3D_FEATURE_LEVEL_11_1,// 2世代前
		D3D_FEATURE_LEVEL_11_0,// 3世代前
	};

	// Direct3Dの初期化
	D3D_FEATURE_LEVEL decidedFeatureLevel = D3D_FEATURE_LEVEL_12_1;
	for (auto flv : fLevels)
	{
		// ReleaseAndGetAddress0fは、dev_のポインタのポインタを表している
		// DirectXの基本オブジェクトとなるD3DDeviceオブジェクトを取得する
		result = D3D12CreateDevice(
			nullptr,// nullptrを入れれば、自動的に
			flv,// フィーチャレベル(DirectXのバージョン)
			IID_PPV_ARGS(dev_.ReleaseAndGetAddressOf())
		);
		if (SUCCEEDED(result))
		{
			decidedFeatureLevel = flv;
			break;
		}
	}
	if (!CheckHRESULT(result))
	{
		return false;
	}

	// コマンドキューの作成
	// コマンドキューとは、コマンドリストをグラボに押し出して実行するもの
	D3D12_COMMAND_QUEUE_DESC cmdQueDesc = {};
	cmdQueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;// フラグなし
	cmdQueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;// 通常順
	cmdQueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;// コマンドリストを普通に実行
	cmdQueDesc.NodeMask = 0;// 基本的に0
	result = dev_->CreateCommandQueue(&cmdQueDesc, IID_PPV_ARGS(cmdQue_.ReleaseAndGetAddressOf()));
	if (!CheckHRESULT(result))
	{
		return false;
	}

	const auto& app = Application::GetInstance();

	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = app.GetWindowWidth();// ウィンドウ幅
	scDesc.Height = app.GetWindowHeight();// ウィンドウ高さ
	scDesc.Stereo = false;// VRの時以外false
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;// アルファは特にいじらない
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;// RGBA 8bitの標準

	// アンチエイリアシングにかかわってる
	scDesc.SampleDesc.Count = 1;// AAなし
	scDesc.SampleDesc.Quality = 0;// AAクオリティ最低
	scDesc.Scaling = DXGI_SCALING_STRETCH;// 画面のサイズを変えると、それに合わせる
	scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;// ウィンドウと全画面を切り替え許容
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	scDesc.BufferCount = 2;// 表画面と裏画面の二枚

	ComPtr<IDXGISwapChain1> swapChain;
	result = dxgi_->CreateSwapChainForHwnd(
		cmdQue_.Get(),
		app.GetWindowHandle(),
		&scDesc,
		nullptr,
		nullptr,
		swapChain.ReleaseAndGetAddressOf());
	if (!CheckHRESULT(result))
	{
		return false;
	}

	result = swapChain.As(&swapChain_);
	if (!CheckHRESULT(result))
	{
		return false;
	}

	return true;
}

HRESULT DirectX12Wrapper::InitDXGI()
{
	// DirectXDeviceというのとDXGIという仕組みがあり、この2つが連携をとって動くが、別物
#ifdef _DEBUG
	constexpr UINT debugFlg = DXGI_CREATE_FACTORY_DEBUG;
#else
	constexpr UINT debugFlg = 0;
#endif

	HRESULT result = S_OK;
	result = CreateDXGIFactory2(debugFlg,		// デバッグの時は有効
		IID_PPV_ARGS(dxgi_.ReleaseAndGetAddressOf()));	// ComPtr内部ポインタのアドレスを渡す

	// resultはいろんな値が帰ってくる
	if (!CheckHRESULT(result))
	{
		return false;
	}

	// 有効なアダプターを検索する
	// アダプター = GPU
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
	return result;
}

void DirectX12Wrapper::ValidateDebugOutput()
{
	// プリプロセッサ(_DEBUG)
#ifdef _DEBUG
	// デバッグレイヤーを有効化
	// DirectX12はデフォルトだとGPU側でエラーが起きても情報がない
	// デバッグレイヤーをオンにすることで、GPU側でエラーが出たらログを出力するように設定する
	ComPtr<ID3D12Debug> debugLayer;
	HRESULT r = D3D12GetDebugInterface(IID_PPV_ARGS(debugLayer.ReleaseAndGetAddressOf()));
	debugLayer->EnableDebugLayer();// デバッグレイヤーを有効にする
	debugLayer.Reset();// デバッグレイヤーオブジェクトを破棄
#endif
}

bool DirectX12Wrapper::InitCommandExecutor()
{
	HRESULT result = S_OK;
	result = dev_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(cmdAlloc_.ReleaseAndGetAddressOf()));
	if (!CheckHRESULT(result))
	{
		return false;
	}


	result = dev_->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAlloc_.Get(),
		nullptr,
		IID_PPV_ARGS(cmdList_.ReleaseAndGetAddressOf()));
	if (!CheckHRESULT(result))
	{
		return false;
	}

	cmdAlloc_->Reset();
	cmdList_->Reset(cmdAlloc_.Get(), nullptr);

	result = dev_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.ReleaseAndGetAddressOf()));

	return true;
}

bool DirectX12Wrapper::CheckHRESULT(HRESULT hResult) const
{
	assert(SUCCEEDED(hResult));
	if (FAILED(hResult))
	{
		return false;
	}

	return true;
}

bool DirectX12Wrapper::CreateBaseConstantBufffer()
{
	const auto& app = Application::GetInstance();
	HRESULT result = S_OK;

	// GPUに転送する情報を確保し、必要な情報をマップする
	int cbSize = sizeof(CBData);//68バイト
	//256バイトアライメントに合わせる
	cbSize = GetAlignmentedValue(cbSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

	CD3DX12_HEAP_PROPERTIES cbHeapProp(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC cbResDesc = CD3DX12_RESOURCE_DESC::Buffer(cbSize);

	//定数バッファの作成
	result = dev_->CreateCommittedResource(
		&cbHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&cbResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constantBuffer_.ReleaseAndGetAddressOf()));
	if (!CheckHRESULT(result))
	{
		return false;
	}
	result = constantBuffer_->Map(0, nullptr, (void**)&mappedCBAdress_);
	if (!CheckHRESULT(result))
	{
		return false;
	}

	XMMATRIX world, camera, proj;

	//使う関数は
	//XMMatrixLookAtLH
	//XMMatrixPerspectiveFovLH
	//です。
	//あと、Y軸回転のためXMMatrixRotationYを使います。

	world = XMMatrixRotationY(XMConvertToRadians(45.0f));

	XMFLOAT3 eye = { 0.0f,5.0f,-4.0f };	// 視点
	XMFLOAT3 target = { 0.0f,0.0f,0.0f };	// 注視点
	XMFLOAT3 up = { 0.0f,1.0f,0.0f };	// 上ベクトル

	//カメラのz表と、ターゲットの座標からカメラの向きを決定する。
	//最後のUPベクトルは(0，1，0)ですが、「仮の上ベクトル」
	//3つ値を入れてますが、内部で座標系を作りたいため重要なのは
	//３軸の「カメラ座標系」を内部で作っている。カメラ座標系といっても
	//現在位置は必要なので、そのためにeyeの座標を渡している
	camera = XMMatrixLookAtLH(XMLoadFloat3(&eye),		//視点
		XMLoadFloat3(&target),	//ターゲット
		XMLoadFloat3(&up));		//上

//遠近法にかかわる計算
	proj = XMMatrixPerspectiveFovLH(
		XM_PIDIV4,//画角
		static_cast<float>(app.GetWindowWidth()) /
		static_cast<float>(app.GetWindowHeight()),//アスペクト比
		0.1f,//Near
		200.0f);//Far
//WVP(ワールド・カメラ・プロジェクション)の順番は守りましょう
	XMStoreFloat4x4(&mappedCBAdress_->mat, world * camera * proj);

	XMStoreFloat4x4(&world_, world);
	XMStoreFloat4x4(&camera_, camera);
	XMStoreFloat4x4(&proj_, proj);

	//mappedCBAdress_->mat = XMFLOAT4X4();//角度を入れる
	//mappedCBAdress_->mat._11 = mappedCBAdress_->mat._22 = mappedCBAdress_->mat._33 = mappedCBAdress_->mat._44 = 1.0f;
	//mappedCBAdress_->mat._11 = 2.0f / static_cast<float>(app->getWindowWidth());
	//mappedCBAdress_->mat._22 = -2.0f / static_cast<float>(app->getWindowHeight());
	//mappedCBAdress_->mat._41 = -1.0f;
	//mappedCBAdress_->mat._42 = 1.0f;

	mappedCBAdress_->time = 0.0f;//時間を入れる

	return true;
}

bool DirectX12Wrapper::LoadTextureAndNormal(const std::wstring& texPath, const std::wstring& normPath)
{
	HRESULT result = S_OK;

	//通常テクスチャのロード
	std::unique_ptr<uint8_t[]> decordedData;//複合化済みデータ
	D3D12_SUBRESOURCE_DATA subresource;//サブリソース
	//LoadWICTextureFromFileは、ストリー時からRAMにロードする
	//しかしまだ、VRAMにはデータが飛んでない状態です
	//かつ、なぜか必要な領域を「確保」
	result = LoadWICTextureFromFile(
		dev_.Get(),//デバイスのポインタ
		texPath.c_str(),//ファイルのパス
		texture_.ReleaseAndGetAddressOf(),//テクスチャリソース(GPUメモリ)
		decordedData,//複合化済みのデータ
		subresource);//サブリソース(ミップマップ等の情報が入ってる)
	if (!CheckHRESULT(result))
	{
		return false;
	}
	//ロードが終わってるのだから、テクスチャバッファの中身は
	//画像ファイルになってるでしょ？←違います。
	//テクスチャバッファは、必要な分、確保されただけです。

	auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(subresource.SlicePitch);
	ComPtr<ID3D12Resource> copySrc;
	result = dev_->CreateCommittedResource(
		&heapType,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(copySrc.ReleaseAndGetAddressOf())
	);
	if (!CheckHRESULT(result))
	{
		return false;
	}

	//updatesuvresourceで、メモリ上のテクスチャデータをVRAMに転送する目入れが
	//出る(内部的にCommandlist)
	auto usres = UpdateSubresources(
		cmdList_.Get(),
		texture_.Get(),//後でテクスチャとして使うVRAM領域
		copySrc.Get(),//マップ→転送するための中間バッファ
		0, 0, 1,//オフセット、最初のデータのインデックス、データ数
		&subresource);//RAM上にある実データ

	result = LoadWICTextureFromFile(
		dev_.Get(),//デバイスのポインタ
		normPath.c_str(),//ファイルのパス
		normTex_.ReleaseAndGetAddressOf(),//テクスチャリソース(GPUメモリ)
		decordedData,//複合化済みのデータ
		subresource);//サブリソース(ミップマップ等の情報が入ってる)
	if (!CheckHRESULT(result))
	{
		return false;
	}

	ComPtr<ID3D12Resource> copySrcN;
	result = dev_->CreateCommittedResource(
		&heapType,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(copySrcN.ReleaseAndGetAddressOf())
	);
	if (!CheckHRESULT(result))
	{
		return false;
	}

	usres = UpdateSubresources(cmdList_.Get(),
		normTex_.Get(),
		copySrcN.Get(),
		0, 0, 1,
		&subresource);

	cmdList_->Close();
	ID3D12CommandList* cmdlists[] = { cmdList_.Get() };
	cmdQue_->ExecuteCommandLists(1, cmdlists);
	cmdQue_->Signal(fence_.Get(), ++fenceValue_);
	while (fence_->GetCompletedValue() != fenceValue_)
	{
		;// 何もしない
	}
	copySrc.Reset();//中間バッファは用無しなので削除

	return true;
}

uint32_t DirectX12Wrapper::GetAlignmentedValue(uint32_t val, uint32_t alignValue)
{
	return val + (alignValue - val % alignValue) % alignValue;
}

bool DirectX12Wrapper::CreateBufferForGLTF(const char* filePath)
{
	auto reader = std::make_shared<GLBStreamReader>();
	auto stream = reader->GetInputStream(filePath);
	auto resReader = std::make_shared<Microsoft::glTF::GLBResourceReader>(std::move(reader), std::move(stream));
	auto document = Microsoft::glTF::Deserialize(resReader->GetJson());

	size_t totalVertexCount = 0;
	size_t totalIndicesCount = 0;
	for (const auto& mesh : document.meshes.Elements())
	{
		for (const auto& primitive : mesh.primitives)
		{
			auto idPos = primitive.GetAttributeAccessorId(Microsoft::glTF::ACCESSOR_POSITION);
			auto& accessPos = document.accessors.Get(idPos);

			auto idIdx = primitive.indicesAccessorId;
			auto& accessIndex = document.accessors.Get(idIdx);

			totalVertexCount += accessPos.count;
			totalIndicesCount += accessIndex.count;
		}
	}

	std::vector<GLBVertex> vertices(totalVertexCount);
	std::vector<uint16_t> indices(totalIndicesCount);

	size_t idx = 0;
	size_t idxIndex = 0;
	for (const auto& mesh : document.meshes.Elements())
	{
		for (const auto& primitive : mesh.primitives)
		{
			// 頂点データの場所を探す
			// 座標
			auto idPos = primitive.GetAttributeAccessorId(Microsoft::glTF::ACCESSOR_POSITION);
			auto& accessPos = document.accessors.Get(idPos);
			auto posData = resReader->ReadBinaryData<float>(document, accessPos);

			// 法線
			auto idNorm = primitive.GetAttributeAccessorId(Microsoft::glTF::ACCESSOR_NORMAL);
			auto& accessNormal = document.accessors.Get(idNorm);
			auto normData = resReader->ReadBinaryData<float>(document, accessNormal);

			// UV値
			bool hasAttributeUV = primitive.HasAttribute(Microsoft::glTF::ACCESSOR_TEXCOORD_0);
			std::vector<float> uvData;
			if (hasAttributeUV)
			{
				auto idUV = primitive.GetAttributeAccessorId(Microsoft::glTF::ACCESSOR_TEXCOORD_0);
				auto& accessUV = document.accessors.Get(idUV);
				uvData = resReader->ReadBinaryData<float>(document, accessUV);
			}

			for (int i = 0; i < accessPos.count; ++i)
			{
				vertices[idx].pos = {posData[i * 3 + 0], posData[i * 3 + 1], posData[i * 3 + 2]};
				vertices[idx].norm = { normData[i * 3 + 0], normData[i * 3 + 1], normData[i * 3 + 2] };
				if (hasAttributeUV)
				{
					vertices[idx].texcoord = { uvData[i * 2 + 0], uvData[i * 2 + 1] };
				}
				
			}
			auto idIdx = primitive.indicesAccessorId;
			auto& accessIndex = document.accessors.Get(idIdx);
			auto idxData = resReader->ReadBinaryData<uint16_t>(document, accessIndex);
			std::copy(idxData.begin(), idxData.end(), indices.begin() + idxIndex);
			++idx;
			idxIndex += idxData.size();
		}
	}
	HRESULT result = CreateMappableOneDimensionalBuffer(sizeof(GLBVertex) * vertices.size(), vertexBuffer_);
	if (FAILED(result))
	{
		return false;
	}
	//GPU上のメモリをいじるためにMap関数を事項する
	//GPU上のメモリは弄れないんだけど、双子メモリを
	//CPUからGPUの内容を弄れるようにする
	GLBVertex* vertMap = nullptr;
	result = vertexBuffer_->Map(0, nullptr, (void**)&vertMap);
	if (!CheckHRESULT(result))
	{
		return false;
	}

	// 頂点データをGPU上のバッファ(CPU上の双子メモリ)にコピーする
	//memcpyみたいな関数ですが、これはSTLの一種で、バッファオーバーランも
	//検出してくれるバージョンなので、こちらを使う
	std::copy(vertices.begin(), vertices.end(), vertMap);
	vertexBuffer_->Unmap(0, nullptr);

	vbView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	//全体のサイズ
	vbView_.SizeInBytes = sizeof(GLBVertex) * vertices.size();
	//Strideは歩幅の意味です。つまり、次のデータまでの距離を示す。
	vbView_.StrideInBytes = sizeof(GLBVertex);

	return true;
}

DirectX12Wrapper::DirectX12Wrapper()
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
}

DirectX12Wrapper::~DirectX12Wrapper()
{

}

bool DirectX12Wrapper::Init(Application* app)
{
	int cbSize = sizeof(CBData);//68バイト
	HRESULT result = S_OK;
	cbSize = GetAlignmentedValue(cbSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

	if (!InitDirectXDevice())
	{
		return false;
	}

	if (!CreateMainRenderTarget())
	{
		return false;
	}

	// コマンド周りの初期化
	if (!InitCommandExecutor())
	{
		return false;
	}

	// 立方体バッファの作成
	//if (!CreateBufferForCube())
	//{
	//	return false;
	//}

	if (!CreateBufferForGLTF("data/snake.glb"))
	{
		return false;
	}

	if (!CreateBaseConstantBufffer())
	{
		return false;
	}

	if (!LoadTextureAndNormal(L"data/neko.png", L"data/neko_n.png"))
	{
		return false;
	}

	//ディスクリプタヒープを作成する
	D3D12_DESCRIPTOR_HEAP_DESC viewDesc = {};
	viewDesc.NodeMask = 0;
	viewDesc.NumDescriptors = 3;//テクスチャ2つ、定数バッファ1つ
	viewDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	viewDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = dev_->CreateDescriptorHeap(&viewDesc,
		IID_PPV_ARGS(viewDescHeap_.ReleaseAndGetAddressOf()));

	assert(SUCCEEDED(result));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;//ミップマップの数
	srvDesc.Format = texture_->GetDesc().Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	auto viewHeapPtr = viewDescHeap_->GetCPUDescriptorHandleForHeapStart();

	//通常テクスチャのビューを作成
	dev_->CreateShaderResourceView(texture_.Get(), &srvDesc,
		viewHeapPtr);

	//ノーマルマップテクスチャのビューを作成
	srvDesc.Format = normTex_->GetDesc().Format;
	viewHeapPtr.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dev_->CreateShaderResourceView(normTex_.Get(), &srvDesc, viewHeapPtr);

	//定数バッファビューを作成
	viewHeapPtr.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = constantBuffer_->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = cbSize;
	dev_->CreateConstantBufferView(&cbvDesc, viewHeapPtr);


	ComPtr<ID3DBlob> vsBlob = nullptr;//頂点シェーダ塊
	ComPtr<ID3DBlob> psBlob = nullptr;//ピクセルシェーダ塊

	//頂点シェーダのロード(ついでにコンパイル)
	result = D3DCompileFromFile(L"VertexShader.hlsl",
		nullptr, nullptr,//define,include
		"main",//エントリポイント(シェーダ内のどの関数を実行するか)
		"vs_5_0",//頂点シェーダとしてコンパイル。シェーダのレベル(最新のとかだと6_1とかなんですが)
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグしたいときはこうする
		0,
		vsBlob.ReleaseAndGetAddressOf(), nullptr);
	assert(SUCCEEDED(result));

	result = D3DCompileFromFile(L"PixelShader.hlsl",
		nullptr, nullptr,
		"main",
		"ps_5_0",//何としてコンパイルするのか(ピクセルシェーダとしてコンパイル)
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		psBlob.ReleaseAndGetAddressOf(),
		nullptr);//エラーが起きた時に、エラー情報が入る(今は使わない)
	assert(SUCCEEDED(result));

	//入力レイアウト
	D3D12_INPUT_ELEMENT_DESC layoutDesc[] = {
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		/*{"COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},*/
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	};

	//パイプラインオブジェクトを作る
	D3D12_GRAPHICS_PIPELINE_STATE_DESC ppDesc = {};
	//頂点入力
	//IA(InputAssembler)
	ppDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	ppDesc.InputLayout.NumElements = _countof(layoutDesc);
	ppDesc.InputLayout.pInputElementDescs = layoutDesc;
	//頂点シェーダ
	//VS(VertexShader)
	ppDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	ppDesc.VS.BytecodeLength = vsBlob->GetBufferSize();

	//ラスタライズ
	ppDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//ppDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//ピクセルシェーダ
	ppDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	ppDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	//出力マージャー
	ppDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	ppDesc.NumRenderTargets = 1;//レンダーターゲットの数
	//これをせってしてないと最終的にレンダーターゲットに書き込まれない
	ppDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	ppDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);//ブレンドステート

	ppDesc.NodeMask = 0;

	ppDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	CD3DX12_DESCRIPTOR_RANGE range[] = {
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0),//t0～t1
		CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0)//b0
	};

	//ルートシグネチャの作成
	D3D12_ROOT_PARAMETER rootParam = {};
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam.DescriptorTable.NumDescriptorRanges = 2;
	rootParam.DescriptorTable.pDescriptorRanges = range;

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = {	};
	samplerDesc.Init(0);
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;

	//ルートシグネチャオブジェクトを作る
	ComPtr<ID3DBlob> rootBlob;//使い捨てのBlob
	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	//頂点入力(頂点レイアウト)があるときはこれを使う。
	//「頂点が入力されますよ」ということだけを教える設定
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rsDesc.NumParameters = 1;//入力されたレジスタ
	rsDesc.pParameters = &rootParam;
	rsDesc.NumStaticSamplers = 1;//サンプラーも
	rsDesc.pStaticSamplers = &samplerDesc;


	//ルートシグネチャ設定をBlobに書き込む
	result = D3D12SerializeRootSignature(&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		rootBlob.ReleaseAndGetAddressOf(),
		nullptr);
	assert(SUCCEEDED(result));

	//このBlobを元に、RootSignature本体を作る
	result = dev_->CreateRootSignature(0, //nodeMaskなので、いつも0
		rootBlob->GetBufferPointer(),//ブロブのアドレス
		rootBlob->GetBufferSize(),//ブロブのサイズ
		IID_PPV_ARGS(rootSig_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	ppDesc.pRootSignature = rootSig_.Get();

	//アンチエリアシングしない
	ppDesc.SampleDesc.Count = 1;
	ppDesc.SampleDesc.Quality = 0;

	result = dev_->CreateGraphicsPipelineState(&ppDesc,
		IID_PPV_ARGS(pipelineState_.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	startTime_ = GetTickCount64();
	lastTime_ = GetTickCount64();

	return true;
}

bool DirectX12Wrapper::Update()
{
	static float xpos = 0.0f;
	static float ypos = 0.0f;
	static float angle = 45.0f;
	mappedCBAdress_->time = static_cast<float>(GetTickCount64() - startTime_) / 1000.0f;
	mappedCBAdress_->mat._41 = -1.0f + xpos / 320.0f;
	mappedCBAdress_->mat._42 = 1.0f + ypos / 240.0f;

	XMMATRIX world, camera, proj;
	world = XMMatrixRotationY(XMConvertToRadians(angle)) * XMMatrixScaling(20.0f, 20.0f, 20.0f);
	camera = XMLoadFloat4x4(&camera_);
	proj = XMLoadFloat4x4(&proj_);
	angle += 1.0f;
	XMStoreFloat4x4(&mappedCBAdress_->mat, world * camera * proj);

	//xpos += 0.1f;

	static float red = 0.0f;

	cmdAlloc_->Reset();
	cmdList_->Reset(cmdAlloc_.Get(), pipelineState_.Get());

	// レンダーターゲットの指定
	auto heapStart = rtvHeaps_->GetCPUDescriptorHandleForHeapStart();
	auto bbIdx = swapChain_->GetCurrentBackBufferIndex();
	heapStart.ptr += bbIdx * dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	auto beforBarrier = CD3DX12_RESOURCE_BARRIER::Transition(rtvResources_[bbIdx],// 対象のリソース(データの塊)
		D3D12_RESOURCE_STATE_PRESENT,// 前の使い方
		D3D12_RESOURCE_STATE_RENDER_TARGET// 後の使い方
	);
	cmdList_->ResourceBarrier(1, &beforBarrier);

	cmdList_->OMSetRenderTargets(1, &heapStart, false, nullptr);
	red = fmodf(red + 0.0001f, 1.0f);
	// その時のレンダーターゲットのクリア
	std::array<float, 4> clearColor = { red, 1.0f, 0.0f, 1.0f };
	cmdList_->ClearRenderTargetView(heapStart, clearColor.data(), 0, nullptr);


	//ルートシグネチャとパイプラインステートをセット
	cmdList_->SetGraphicsRootSignature(rootSig_.Get());
	cmdList_->SetPipelineState(pipelineState_.Get());
	ID3D12DescriptorHeap* heaps[] = {
		viewDescHeap_.Get()//テクスチャバッファビューが先頭にあるヒープ
	};
	//テクスチャビューをセット
	cmdList_->SetDescriptorHeaps(1, heaps);

	//ルートパラメータの0番
	cmdList_->SetGraphicsRootDescriptorTable(0, viewDescHeap_->GetGPUDescriptorHandleForHeapStart());


	// 頂点バッファをセットする
	cmdList_->IASetVertexBuffers(
		0,//スロット
		1,//頂点バッファビュー配列の数
		&vbView_);//頂点バッファビューの本体

	cmdList_->IASetIndexBuffer(&ibView_);//頂点バッファビューの本体


	//プリミティブトポロジーの設定
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);


	auto& app = Application::GetInstance();


	//どこに描画するのか指定する。
	auto viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, app.GetWindowWidth(), app.GetWindowHeight());
	cmdList_->RSSetViewports(1, &viewport);

	D3D12_RECT rect = {};
	rect.left = 0;
	rect.top = 0;
	rect.right = app.GetWindowWidth();
	rect.bottom = app.GetWindowHeight();
	cmdList_->RSSetScissorRects(1, &rect);

	//三頂点を描画する
	//cmdList_->DrawIndexedInstanced(36,//インデックス数
	//	1,//インスタンス数
	//	0,//インデックスオフセット
	//	0,//頂点オフセット
	//	0);//インスタンスオフセット

	auto vnum = vbView_.SizeInBytes / vbView_.StrideInBytes;
	cmdList_->DrawInstanced(vnum, 1, 0, 0);

	auto afterBarrier = CD3DX12_RESOURCE_BARRIER::Transition(rtvResources_[bbIdx],// 対象のリソース(データの塊)
		D3D12_RESOURCE_STATE_RENDER_TARGET,// 前の使い方    
		D3D12_RESOURCE_STATE_PRESENT// 後の使い方
	);
	cmdList_->ResourceBarrier(1, &afterBarrier);

	cmdList_->Close();

	ID3D12CommandList* cmdlists[] = { cmdList_.Get() };
	cmdQue_->ExecuteCommandLists(1, cmdlists);

	// GPUの処理が終わったとき、fence_の中の値が、fenceValue_に変化する
	// 例)初回なら、最初fence_は0で初期化されているので0。ここで、
	// ++fenceValueを渡しているため、GPU上の処理が終わったらfence_の中の値は1になる
	cmdQue_->Signal(fence_.Get(), ++fenceValue_);
	while (fence_->GetCompletedValue() != fenceValue_)
	{
		;// 何もしない
	}

	swapChain_->Present(0, 0);
	auto tickCount = GetTickCount64();
	while (lastTime_ + 16 >= tickCount) {
		tickCount = GetTickCount64();
		Sleep(1);
	}
	lastTime_ = tickCount;
	return false;
}



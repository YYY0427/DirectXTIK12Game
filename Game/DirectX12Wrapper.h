#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <array>
#include <vector>
#include <string>

using Microsoft::WRL::ComPtr;

class Application;

class DirectX12Wrapper
{
private:
	ComPtr<ID3D12Device> dev_;						//DX12デバイス
	ComPtr<IDXGIFactory6> dxgi_;					//DXGI
	ComPtr<IDXGISwapChain4> swapChain_;				//スワップチェーン
	ComPtr<ID3D12CommandQueue> cmdQue_;				//コマンドキュー
	ComPtr<ID3D12GraphicsCommandList> cmdList_;		//コマンドリスト
	ComPtr<ID3D12CommandAllocator> cmdAlloc_;		//コマンドアロケータ
	ComPtr<ID3D12DescriptorHeap> rtvHeaps_;			//レンダーターゲットビュー用デスクリプタ
	ComPtr<ID3D12Fence> fence_;						//「待ち」を実装するためのもの
	std::array<ID3D12Resource*, 2> rtvResources_;
	ComPtr<ID3D12Resource> vertexBuffer_;			//頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};			//頂点バッファビュー
	ComPtr<ID3D12Resource> indexBuffer_;	// インデックスバッファ
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};	// インデックスバッファビュー

	ComPtr<ID3D12RootSignature> rootSig_;			//ルートシグネチャ
	ComPtr<ID3D12PipelineState> pipelineState_;		//パイプラインステート

	ComPtr<ID3D12DescriptorHeap> viewDescHeap_;		//テクスチャ用ディスクリプタヒープ
	ComPtr<ID3D12Resource> texture_;				//テクスチャバッファ
	ComPtr<ID3D12Resource> normTex_;				//テクスチャバッファ(法線マップ)

	//定数バッファデータ基本型
	struct CBData {
		DirectX::XMFLOAT4X4 mat;					//座標変換用
		float time;									//時間
	};

	//定数バッファまわり
	ComPtr <ID3D12Resource> constantBuffer_;		//定数バッファ
	CBData* mappedCBAdress_ = nullptr;				//定数バッファのマップ用
	ULONGLONG startTime_ = 0;						//時間経過計測用
	ULONGLONG lastTime_ = 0;						//時間経過計測用
	UINT64 fenceValue_ = 0;

	// 3D
	DirectX::XMFLOAT4X4 world_;//オブジェクトの回転
	DirectX::XMFLOAT4X4 camera_;//カメラ行列
	DirectX::XMFLOAT4X4 proj_;//遠近法行列(射影行列3D→2D)


	/// <summary>
	/// 渡された数値を特定のバイト境界に合わせた値を返す
	/// </summary>
	/// <param name="val">元の値</param>
	/// <param name="alignValue">合わせたい値</param>
	/// <returns></returns>
	uint32_t GetAlignmentedValue(uint32_t val, uint32_t alignValue);

	//頂点データ構造体
	struct Vertex {
		DirectX::XMFLOAT3 pos;//座標
		DirectX::XMFLOAT3 col;//カラー
		DirectX::XMFLOAT2 texcoord;//UV値
		Vertex() {};
		Vertex(const DirectX::XMFLOAT3& p, const DirectX::XMFLOAT3& c, const DirectX::XMFLOAT2& uv) :
			pos(p), col(c), texcoord(uv) {}
	};

	// コードの整理のために追加した関数
	std::vector<Vertex> CreateCubeVertices() const;		// 立方体のために頂点配列を作って返す
	std::vector<uint32_t> CreateCubeIndices() const;	// 立方体のためのインデックス配列を作って返す

	// 頂点やインデックスなどに使う１次元バッファを作成して第二引数に渡す
	HRESULT CreateMappableOneDimensionalBuffer(size_t bufferSize, ComPtr<ID3D12Resource>& res);

	// 内部で立方体のための頂点バッファとインデックスバッファを作ります。
	bool CreateBufferForCube();

	// デバッグ出力を有効にする
	void ValidateDebugOutput();

	// DXGI周りの初期化
	HRESULT InitDXGI();

	// DirectXまわりの初期化
	bool InitDirectXDevice();

	// レンダーターゲットビューの作成＆設定
	bool CreateMainRenderTarget();

	// コマンド周りの初期化
	bool InitCommandExecutor();

	// HRESULT戻り値チェック
	bool CheckHRESULT(HRESULT hResult) const;

	// 基本として使う定数バッファを定義しGPUに送る
	bool CreateBaseConstantBufffer();

	// 通常とノーマルテクスチャをロードする
	// (GPU転送まで込み)
	// @param texPath 通常画像のパス
	// @param normPath 法線マップパス
	bool LoadTextureAndNormal(const std::wstring& texPath, const std::wstring& normPath);

	// GLTFデータを読み込んでバッファを作る
	bool CreateBufferForGLTF(const char* filePath);

	//頂点データ構造体
	struct GLBVertex {
		DirectX::XMFLOAT3 pos;//座標
		DirectX::XMFLOAT3 norm;//法線
		DirectX::XMFLOAT2 texcoord;//UV値
		GLBVertex() {};
		GLBVertex(const DirectX::XMFLOAT3& p, const DirectX::XMFLOAT3& n, const DirectX::XMFLOAT2& uv) :
			pos(p), norm(n), texcoord(uv) {}
	};
public:
	DirectX12Wrapper();
	~DirectX12Wrapper();
	bool Init(Application* app);
	bool Update();
};


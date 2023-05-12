#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <array>

class Application;

using Microsoft::WRL::ComPtr;

class DirectX12Wrapper
{
public:
	DirectX12Wrapper();
	~DirectX12Wrapper();
	bool Init(Application* app);
	bool Update();

private:
	ComPtr<ID3D12Device> dev_;					// DX12のデバイス
	ComPtr<IDXGIFactory6> dxgi_;				// DXGIのデバイス
	ComPtr<IDXGISwapChain4> swapChain_;			// スワップチェーン
	ComPtr<ID3D12CommandQueue> cmdQue_;			// コマンドキュー
	ComPtr<ID3D12GraphicsCommandList> cmdList_;	// コマンドリスト
	ComPtr<ID3D12CommandAllocator> cmdAlloc_;	// コマンドアロケータ
	ComPtr<ID3D12DescriptorHeap> rtvHeaps_;		// レンダーターゲットビュー用デスクリプタヒープ
	ComPtr<ID3D12Fence> fence_;					// 「待ち」を実装するためのもの
	std::array<ID3D12Resource*, 2> rtvResources_;
	ComPtr<ID3D12Resource> vertexBuffer_;		// 頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};

	UINT64 fenceValue_ = 0;
};
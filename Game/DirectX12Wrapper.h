#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>

class Application;

using Microsoft::WRL::ComPtr;

class DirectX12Wrapper
{
public:
	DirectX12Wrapper();
	~DirectX12Wrapper();
	bool Init(Application* app);

private:
	ComPtr<ID3D12Device> dev_;		// DX12のデバイス
	ComPtr<IDXGIFactory6> dxgi_;	// DXGIのデバイス
	ComPtr<IDXGISwapChain4> swapChain_;		// スワップチェーン
	ComPtr<ID3D12CommandQueue> cmdQue_;		// コマンドキュー
	ComPtr<ID3D12DescriptorHeap> rtvHeaps_;	// レンダーターゲットビュー用デスクリプタヒープ
};
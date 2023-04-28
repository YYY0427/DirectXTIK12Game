#pragma once
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>

using Microsoft::WRL::ComPtr;

class DirectX12Wrapper
{
public:
	DirectX12Wrapper();
	~DirectX12Wrapper();
	bool Init();

private:
	ComPtr<ID3D12Device> dev_;	// DX12のデバイス
};
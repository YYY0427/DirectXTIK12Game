#include "DirectX12Wrapper.h"
#include "Application.h"
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

DirectX12Wrapper::DirectX12Wrapper()
{
}

DirectX12Wrapper::~DirectX12Wrapper()
{

}

bool DirectX12Wrapper::Init()
{
	HRESULT result = D3D12CreateDevice(
								nullptr,
								D3D_FEATURE_LEVEL_12_0,
								IID_PPV_ARGS(dev_.ReleaseAndGetAddressOf())
					);
	assert(SUCCEEDED(result));

	return false;
}

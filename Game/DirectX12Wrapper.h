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
	ComPtr<ID3D12Device> dev_;					// DX12�̃f�o�C�X
	ComPtr<IDXGIFactory6> dxgi_;				// DXGI�̃f�o�C�X
	ComPtr<IDXGISwapChain4> swapChain_;			// �X���b�v�`�F�[��
	ComPtr<ID3D12CommandQueue> cmdQue_;			// �R�}���h�L���[
	ComPtr<ID3D12GraphicsCommandList> cmdList_;	// �R�}���h���X�g
	ComPtr<ID3D12CommandAllocator> cmdAlloc_;	// �R�}���h�A���P�[�^
	ComPtr<ID3D12DescriptorHeap> rtvHeaps_;		// �����_�[�^�[�Q�b�g�r���[�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12Fence> fence_;					// �u�҂��v���������邽�߂̂���
	std::array<ID3D12Resource*, 2> rtvResources_;
	ComPtr<ID3D12Resource> vertexBuffer_;		// ���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};

	UINT64 fenceValue_ = 0;
};
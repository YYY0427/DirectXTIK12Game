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
	ComPtr<ID3D12Device> dev_;						//DX12�f�o�C�X
	ComPtr<IDXGIFactory6> dxgi_;					//DXGI
	ComPtr<IDXGISwapChain4> swapChain_;				//�X���b�v�`�F�[��
	ComPtr<ID3D12CommandQueue> cmdQue_;				//�R�}���h�L���[
	ComPtr<ID3D12GraphicsCommandList> cmdList_;		//�R�}���h���X�g
	ComPtr<ID3D12CommandAllocator> cmdAlloc_;		//�R�}���h�A���P�[�^
	ComPtr<ID3D12DescriptorHeap> rtvHeaps_;			//�����_�[�^�[�Q�b�g�r���[�p�f�X�N���v�^
	ComPtr<ID3D12Fence> fence_;						//�u�҂��v���������邽�߂̂���
	std::array<ID3D12Resource*, 2> rtvResources_;
	ComPtr<ID3D12Resource> vertexBuffer_;			//���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};			//���_�o�b�t�@�r���[
	ComPtr<ID3D12Resource> indexBuffer_;	// �C���f�b�N�X�o�b�t�@
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};	// �C���f�b�N�X�o�b�t�@�r���[

	ComPtr<ID3D12RootSignature> rootSig_;			//���[�g�V�O�l�`��
	ComPtr<ID3D12PipelineState> pipelineState_;		//�p�C�v���C���X�e�[�g

	ComPtr<ID3D12DescriptorHeap> viewDescHeap_;		//�e�N�X�`���p�f�B�X�N���v�^�q�[�v
	ComPtr<ID3D12Resource> texture_;				//�e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> normTex_;				//�e�N�X�`���o�b�t�@(�@���}�b�v)

	//�萔�o�b�t�@�f�[�^��{�^
	struct CBData {
		DirectX::XMFLOAT4X4 mat;					//���W�ϊ��p
		float time;									//����
	};

	//�萔�o�b�t�@�܂��
	ComPtr <ID3D12Resource> constantBuffer_;		//�萔�o�b�t�@
	CBData* mappedCBAdress_ = nullptr;				//�萔�o�b�t�@�̃}�b�v�p
	ULONGLONG startTime_ = 0;						//���Ԍo�ߌv���p
	ULONGLONG lastTime_ = 0;						//���Ԍo�ߌv���p
	UINT64 fenceValue_ = 0;

	// 3D
	DirectX::XMFLOAT4X4 world_;//�I�u�W�F�N�g�̉�]
	DirectX::XMFLOAT4X4 camera_;//�J�����s��
	DirectX::XMFLOAT4X4 proj_;//���ߖ@�s��(�ˉe�s��3D��2D)


	/// <summary>
	/// �n���ꂽ���l�����̃o�C�g���E�ɍ��킹���l��Ԃ�
	/// </summary>
	/// <param name="val">���̒l</param>
	/// <param name="alignValue">���킹�����l</param>
	/// <returns></returns>
	uint32_t GetAlignmentedValue(uint32_t val, uint32_t alignValue);

	//���_�f�[�^�\����
	struct Vertex {
		DirectX::XMFLOAT3 pos;//���W
		DirectX::XMFLOAT3 col;//�J���[
		DirectX::XMFLOAT2 texcoord;//UV�l
		Vertex() {};
		Vertex(const DirectX::XMFLOAT3& p, const DirectX::XMFLOAT3& c, const DirectX::XMFLOAT2& uv) :
			pos(p), col(c), texcoord(uv) {}
	};

	// �R�[�h�̐����̂��߂ɒǉ������֐�
	std::vector<Vertex> CreateCubeVertices() const;		// �����̂̂��߂ɒ��_�z�������ĕԂ�
	std::vector<uint32_t> CreateCubeIndices() const;	// �����̂̂��߂̃C���f�b�N�X�z�������ĕԂ�

	// ���_��C���f�b�N�X�ȂǂɎg���P�����o�b�t�@���쐬���đ������ɓn��
	HRESULT CreateMappableOneDimensionalBuffer(size_t bufferSize, ComPtr<ID3D12Resource>& res);

	// �����ŗ����̂̂��߂̒��_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@�����܂��B
	bool CreateBufferForCube();

	// �f�o�b�O�o�͂�L���ɂ���
	void ValidateDebugOutput();

	// DXGI����̏�����
	HRESULT InitDXGI();

	// DirectX�܂��̏�����
	bool InitDirectXDevice();

	// �����_�[�^�[�Q�b�g�r���[�̍쐬���ݒ�
	bool CreateMainRenderTarget();

	// �R�}���h����̏�����
	bool InitCommandExecutor();

	// HRESULT�߂�l�`�F�b�N
	bool CheckHRESULT(HRESULT hResult) const;

	// ��{�Ƃ��Ďg���萔�o�b�t�@���`��GPU�ɑ���
	bool CreateBaseConstantBufffer();

	// �ʏ�ƃm�[�}���e�N�X�`�������[�h����
	// (GPU�]���܂ō���)
	// @param texPath �ʏ�摜�̃p�X
	// @param normPath �@���}�b�v�p�X
	bool LoadTextureAndNormal(const std::wstring& texPath, const std::wstring& normPath);

	// GLTF�f�[�^��ǂݍ���Ńo�b�t�@�����
	bool CreateBufferForGLTF(const char* filePath);

	//���_�f�[�^�\����
	struct GLBVertex {
		DirectX::XMFLOAT3 pos;//���W
		DirectX::XMFLOAT3 norm;//�@��
		DirectX::XMFLOAT2 texcoord;//UV�l
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


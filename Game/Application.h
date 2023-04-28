#pragma once
#include <Windows.h>
#include <memory>

class DirectX12Wrapper;

/// <summary>
/// �A�v���S�̂Ŏg���A�v���V���O���g���N���X
/// </summary>
class Application
{
public:
	static Application& GetInstance();

	/// <summary>
	/// DirectX�A�v���P�[�V�������J�n����
	/// </summary>
	void Run();
	HINSTANCE GetInstanceHandle();
	HWND GetWindowHandle();

	~Application();

private:
	// �R�s�[�A����֎~
	Application();
	Application(const Application&) = delete;
	void operator = (const Application&) = delete;

private:
	std::shared_ptr<DirectX12Wrapper> dx12_;
	HINSTANCE instance_;	// �A�v���P�[�V�����C���X�^���X
	HWND wHandle_;			// �E�B���h�E�n���h��
};
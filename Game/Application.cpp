#include "Application.h"
#include <DirectXHelpers.h>
#include <Windows.h>
#include <cassert>

/// <summary>
/// �E�B���h�E�v���V�[�W��(OS����̃R�[���o�b�N�֐�)
/// </summary>
/// <param name="hWnd"></param>
/// <param name="msg"></param>
/// <param name="wparam"></param>
/// <param name="param"></param>
/// <returns></returns>
LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM param)
{
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);		// ���̃A�v���P�[�V�����͏I���
		return 0;
	}
	return DefWindowProc(hWnd, msg, wparam, param);
}

namespace
{
	constexpr int window_width = 640;
	constexpr int window_height = 480;
	const wchar_t class_name[] = L"GameWindow";
}

Application& Application::GetInstance()
{
	static Application instance;
	return instance;
}

void Application::Run()
{
	ShowWindow(wHandle_, SW_SHOW);
	MSG msg = {};
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);		// ���b�Z�[�W�̖|��(���z�L�[����ϊ�)
			DispatchMessage(&msg);		// �g��Ȃ��������b�Z�[�W�����ɉ�
		}
		if (msg.message == WM_QUIT)
		{
			break;
		}
	}
}

Application::~Application()
{
	UnregisterClass(class_name, instance_);
}

Application::Application()
{
	instance_ = GetModuleHandle(0);
	WNDCLASSEX wc = {};
	wc.hInstance = instance_;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.lpszClassName = class_name;		// OS�����ʂ��邽�߂̖��O

	// WindowsOS�ɑ΂��āu���̃E�B���h�E�N���X���g������A�������Ƃ��Ăˁv
	// �E�B���h�N���X��OS�ɓo�^����
	auto result = RegisterClassEx(&wc);
	assert(result != 0);

	RECT wrc = { 0, 0, window_width, window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	
	wHandle_ = CreateWindow(
		wc.lpszClassName,			// OS�����ʂ��邽�߂̖��O
		L"GameWindow Okome",		// �^�C�g���o�[�̕�����
		WS_OVERLAPPEDWINDOW,		// ���ʂɏd�˂���E�B���h�E�̂���
		CW_USEDEFAULT,				// X���W���f�t�H���g�ɂ���
		CW_USEDEFAULT,				// X���W���f�t�H���g�ɂ���
		wrc.right - wrc.left,		// �E�B���h�E��
		wrc.bottom - wrc.top,		// �E�B���h�E����
		nullptr,					// �e�E�B���h�E�n���h��(�e���Ȃ��̂�nullptr)
		nullptr,					// ���j���[�E�B���h�E�n���h��(���j���[���Ȃ��̂�nullptr)
		wc.hInstance,				// �E�B���h�E�쐬���̃A�v���̃n���h��
		nullptr						// �ǉ��̃A�v���P�[�V�����f�[�^(�g��Ȃ��̂�nullptr)			
	);
	assert(wHandle_ != NULL);
}
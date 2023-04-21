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
	return DefWindowProc(hWnd, msg, wparam, param);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR cmd, int num)
{
	WNDCLASSEX wc = {};
	wc.hInstance = hInstance;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.lpszClassName = L"GameWindow";		// OS�����ʂ��邽�߂̖��O

	// WindowsOS�ɑ΂��āu���̃E�B���h�E�N���X���g������A�������Ƃ��Ăˁv
	// �E�B���h�N���X��OS�ɓo�^����
	auto result = RegisterClassEx(&wc);
	assert(result != 0);

	HWND hWnd;
	hWnd = CreateWindow(
		wc.lpszClassName,			// OS�����ʂ��邽�߂̖��O
		L"GameWindow Okome",		// �^�C�g���o�[�̕�����
		WS_OVERLAPPEDWINDOW,		// ���ʂɏd�˂���E�B���h�E�̂���
		CW_USEDEFAULT,				// X���W���f�t�H���g�ɂ���
		CW_USEDEFAULT,				// X���W���f�t�H���g�ɂ���
		640,						// �E�B���h�E��
		480,						// �E�B���h�E����
		nullptr,					// �e�E�B���h�E�n���h��(�e���Ȃ��̂�nullptr)
		nullptr,					// ���j���[�E�B���h�E�n���h��(���j���[���Ȃ��̂�nullptr)
		wc.hInstance,				// �E�B���h�E�쐬���̃A�v���̃n���h��
		nullptr						// �ǉ��̃A�v���P�[�V�����f�[�^(�g��Ȃ��̂�nullptr)			
	);
	assert(hWnd != NULL);

	ShowWindow(hWnd, SW_SHOW);
	MSG msg = {};
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);		// ���b�Z�[�W�̖|��(���z�L�[����ϊ�)
			DispatchMessage(&msg);		// �g��Ȃ��������b�Z�[�W�����ɉ�
		}
	}
	UnregisterClass(wc.lpszClassName, wc.hInstance);
	return 0;
}
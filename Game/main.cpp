#include "Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR cmd, int num)
{
	auto& app = Application::GetInstance();
	app.Init();	// �A�v���P�[�V�����̏�������
	app.Run();	// �A�v���P�[�V�����̃��C�����[�v���s
	return 0;
}
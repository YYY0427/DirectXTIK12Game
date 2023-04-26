#include "Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR cmd, int num)
{
	auto& app = Application::GetInstance();
	app.Run();
	return 0;
}
#include "Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR cmd, int num)
{
	auto& app = Application::GetInstance();
	app.Init();	// アプリケーションの初期化と
	app.Run();	// アプリケーションのメインループ実行
	return 0;
}
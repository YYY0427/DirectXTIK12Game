#include <DirectXHelpers.h>
#include <Windows.h>
#include <cassert>

/// <summary>
/// ウィンドウプロシージャ(OSからのコールバック関数)
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
	wc.lpszClassName = L"GameWindow";		// OSが識別するための名前

	// WindowsOSに対して「このウィンドウクラスを使うから、準備しといてね」
	// ウィンドクラスをOSに登録する
	auto result = RegisterClassEx(&wc);
	assert(result != 0);

	HWND hWnd;
	hWnd = CreateWindow(
		wc.lpszClassName,			// OSが識別するための名前
		L"GameWindow Okome",		// タイトルバーの文字列
		WS_OVERLAPPEDWINDOW,		// 普通に重ねられるウィンドウのこと
		CW_USEDEFAULT,				// X座標をデフォルトにする
		CW_USEDEFAULT,				// X座標をデフォルトにする
		640,						// ウィンドウ幅
		480,						// ウィンドウ高さ
		nullptr,					// 親ウィンドウハンドル(親がないのでnullptr)
		nullptr,					// メニューウィンドウハンドル(メニューもないのでnullptr)
		wc.hInstance,				// ウィンドウ作成元のアプリのハンドル
		nullptr						// 追加のアプリケーションデータ(使わないのでnullptr)			
	);
	assert(hWnd != NULL);

	ShowWindow(hWnd, SW_SHOW);
	MSG msg = {};
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);		// メッセージの翻訳(仮想キー等を変換)
			DispatchMessage(&msg);		// 使わなかったメッセージを次に回す
		}
	}
	UnregisterClass(wc.lpszClassName, wc.hInstance);
	return 0;
}
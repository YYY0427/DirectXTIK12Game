#include "Application.h"
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
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);		// このアプリケーションは終わり
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
			TranslateMessage(&msg);		// メッセージの翻訳(仮想キー等を変換)
			DispatchMessage(&msg);		// 使わなかったメッセージを次に回す
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
	wc.lpszClassName = class_name;		// OSが識別するための名前

	// WindowsOSに対して「このウィンドウクラスを使うから、準備しといてね」
	// ウィンドクラスをOSに登録する
	auto result = RegisterClassEx(&wc);
	assert(result != 0);

	RECT wrc = { 0, 0, window_width, window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	
	wHandle_ = CreateWindow(
		wc.lpszClassName,			// OSが識別するための名前
		L"GameWindow Okome",		// タイトルバーの文字列
		WS_OVERLAPPEDWINDOW,		// 普通に重ねられるウィンドウのこと
		CW_USEDEFAULT,				// X座標をデフォルトにする
		CW_USEDEFAULT,				// X座標をデフォルトにする
		wrc.right - wrc.left,		// ウィンドウ幅
		wrc.bottom - wrc.top,		// ウィンドウ高さ
		nullptr,					// 親ウィンドウハンドル(親がないのでnullptr)
		nullptr,					// メニューウィンドウハンドル(メニューもないのでnullptr)
		wc.hInstance,				// ウィンドウ作成元のアプリのハンドル
		nullptr						// 追加のアプリケーションデータ(使わないのでnullptr)			
	);
	assert(wHandle_ != NULL);
}
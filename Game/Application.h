#pragma once
#include <Windows.h>

class Application
{
public:
	static Application& GetInstance();

	/// <summary>
	/// DirectXアプリケーションを開始する
	/// </summary>
	void Run();

	~Application();

private:
	// コピー、代入禁止
	Application();
	Application(const Application&) = delete;
	void operator = (const Application&) = delete;

private:
	HINSTANCE instance_;
	HWND wHandle_;
};
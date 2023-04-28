#pragma once
#include <Windows.h>
#include <memory>

class DirectX12Wrapper;

/// <summary>
/// アプリ全体で使うアプリシングルトンクラス
/// </summary>
class Application
{
public:
	static Application& GetInstance();

	/// <summary>
	/// DirectXアプリケーションを開始する
	/// </summary>
	void Run();
	HINSTANCE GetInstanceHandle();
	HWND GetWindowHandle();

	~Application();

private:
	// コピー、代入禁止
	Application();
	Application(const Application&) = delete;
	void operator = (const Application&) = delete;

private:
	std::shared_ptr<DirectX12Wrapper> dx12_;
	HINSTANCE instance_;	// アプリケーションインスタンス
	HWND wHandle_;			// ウィンドウハンドル
};
#pragma once
#include <Windows.h>

class Application
{
public:
	static Application& GetInstance();

	/// <summary>
	/// DirectX�A�v���P�[�V�������J�n����
	/// </summary>
	void Run();

	~Application();

private:
	// �R�s�[�A����֎~
	Application();
	Application(const Application&) = delete;
	void operator = (const Application&) = delete;

private:
	HINSTANCE instance_;
	HWND wHandle_;
};
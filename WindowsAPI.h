#pragma once
#include<Windows.h>

class WindowsAPI
{
public:
	static const int winW = 1280;
	static const int winH = 720;
	WNDCLASSEX w{};//�E�B���h�E�N���X
	RECT wrc{};//�E�B���h�E�T�C�Y
	HWND hwnd{};//�E�B���h�E�I�u�W�F�N�g


public:
	WindowsAPI();
	~WindowsAPI();
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	void Initialize();
	void Finalize();
	HWND GetHwnd()const { return hwnd; }
	HINSTANCE GetHInstance()const { return w.hInstance; }
	bool ProcessMessage();
};


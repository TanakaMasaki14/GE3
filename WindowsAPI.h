#pragma once
#include<Windows.h>

class WindowsAPI
{
public:
	static const int winW = 1280;
	static const int winH = 720;
	WNDCLASSEX w{};//ウィンドウクラス
	RECT wrc{};//ウィンドウサイズ
	HWND hwnd{};//ウィンドウオブジェクト


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


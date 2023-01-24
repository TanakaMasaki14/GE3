#pragma once
#include <Windows.h>
#include <d3d12.h>
#pragma comment(lib,"winmm.lib")


class WindowsApp final
{
private:
	WNDCLASSEX w;
	HWND hwnd;


	//ウインドウプロシージャ
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	WindowsApp();

	~WindowsApp() { ; }


public:
	//ウィンドウサイズ
	const float window_width = 1280.0f;
	const float window_height = 720.0f;

	// ビューポート設定コマンド
	D3D12_VIEWPORT viewport;


	//コピーコンストラクタを無効
	WindowsApp(const WindowsApp& obj) = delete;
	//代入演算子も
	WindowsApp& operator=(const WindowsApp& obj) = delete;

	static WindowsApp& GetInstance();

	bool MessegeRoop(MSG msg);

	void UnregisterClassA()
	{
		UnregisterClass(w.lpszClassName, w.hInstance);
	}

	//void operator=(const WindowsApp& win)
	//{
	//	*this = win;
	//}

	//getter
	WNDCLASSEX Getw() { return w; }
	HWND Gethwnd() { return hwnd; }
};


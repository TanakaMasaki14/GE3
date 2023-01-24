#include "WindowsApp.h"
#include<imgui_impl_win32.h>


// imgui専用のウィンドウプロシージャ
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam,
	LPARAM lParam);


LRESULT WindowsApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	//imgui用のウィンドウプロシージャ呼び出し
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
	{
		return true;
	}

	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの修了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

WindowsApp::WindowsApp()
{
	//システムタイマーの分解能を上げる
	timeBeginPeriod(1);

	//ウインドウクラスの設定
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;		//ウインドウプロシージャの設定
	w.lpszClassName = L"DirectX";				//ウインドウクラス名
	w.hInstance = GetModuleHandle(nullptr);		//ウインドウハンドル
	w.hCursor = LoadCursor(NULL, IDC_ARROW);	//カーソル指定

	//ウインドウクラスをOSに登録する
	RegisterClassEx(&w);
	//ウインドウサイズ{X,Y,横幅,縦幅}
	RECT wrc = { 0,0,(LONG)window_width,(LONG)window_height };
	//自動でサイズ補正
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウオブジェクトの生成
	hwnd = CreateWindow(
		w.lpszClassName,		//クラス名
		L"LE2A_04_イイダ_ユウキ",			//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	//標準的なウィンドウスタイル
		CW_USEDEFAULT,			//表示X座標(OSに任せる)
		CW_USEDEFAULT,			//表示Y座標(OSに任せる)
		wrc.right - wrc.left,	//ウィンドウ横幅
		wrc.bottom - wrc.top,	//ウィンドウ縦幅
		nullptr,				//親ウィンドウハンドル
		nullptr,				//メニューハンドル
		w.hInstance,			//呼び出しアプリケーションハンドル
		nullptr					//オプション
	);

	//ウインドウを表示状態にする
	ShowWindow(hwnd, SW_SHOW);

	viewport = { 0, 0, window_width, window_height, 0.0f, 1.0f };
}

WindowsApp& WindowsApp::GetInstance()
{
	static WindowsApp inst; // private なコンストラクタを呼び出す。
	return inst;
}

bool WindowsApp::MessegeRoop(MSG msg)
{
	//メッセージがあるか?
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);	//キー入力メッセージの処理
		DispatchMessage(&msg);	//プロシージャにメッセージを送る
	}

	//×ボタンで終了
	if (msg.message == WM_QUIT) {
		return true;
	}

	return false;
}

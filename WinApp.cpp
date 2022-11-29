#include "WinApp.h"
#pragma comment(lib,"winmm.lib")

//ウィンドウプロシージャ
LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
		//ウィンドウ破棄されたなら
	case WM_DESTROY:
		//OSに対してアプリ終了を通知
		PostQuitMessage(0);
		return 0;
	}

	//メッセージ処理
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WinApp::Initialize() {
	//クラス設定
	w_.cbSize = sizeof(WNDCLASSEX);
	w_.lpfnWndProc = (WNDPROC)WindowProc;
	w_.lpszClassName = L"DirectXGame";
	w_.hInstance = GetModuleHandle(nullptr);
	w_.hCursor = LoadCursor(NULL, IDC_ARROW);

	//OSに登録
	RegisterClassEx(&w_);
	//サイズ
	RECT wrc = { 0,0,WinWidth,WinHeight };
	//自動でサイズ修正
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウハンドル
	hwnd_ = CreateWindow(w_.lpszClassName,
		L"LE2B_14_タナカ_マサキ_GE3",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		w_.hInstance,
		nullptr);

	//表示状態にする
	ShowWindow(hwnd_, SW_SHOW);

	//システムタイマーの分解能を上げる
	timeBeginPeriod(1);
}

void WinApp::Update() {
}

void WinApp::Finalize() {
	//クラス登録を解除
	UnregisterClass(w_.lpszClassName, w_.hInstance);
}

bool WinApp::ProcessMessage() {
	MSG msg{};

	//メッセージはあるか？
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//×で終了メッセージが来たらループを抜ける
	if (msg.message == WM_QUIT) {
		return true;
	}

	return false;
}

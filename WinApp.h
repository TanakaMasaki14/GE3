#pragma once
#include <Windows.h>

class WinApp {
public: //静的メンバ関数(static)

	static LRESULT WindowProc(
		HWND hwnd,
		UINT msg,
		WPARAM wparam,
		LPARAM lparam);

public: //基本的なメンバ関数

	//初期化
	void Initialize();
	//更新
	void Update();
	//終了
	void Finalize();

public: //固有のメンバ関数

	//アプリ終了メッセージがあるか
	bool ProcessMessage();

public: //ゲッター

	HINSTANCE GetHInstance() const { return w_.hInstance; }
	HWND GetHwnd() const { return hwnd_; }

public: //外部で使える定数

	//サイズ
	static const int WinWidth = 1280;
	static const int WinHeight = 720;

private: //よく使うメンバ変数

	//クラス設定
	WNDCLASSEX w_{};
	//ウィンドウハンドル
	HWND hwnd_ = nullptr;

};
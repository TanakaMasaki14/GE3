#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include "WindowsApp.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include <wrl.h>


class KeyboardInput final
{
private:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	//全キーの入力状態を取得する
	BYTE key[256] = {};
	BYTE oldkey[256] = {};
	HRESULT result;

	//DirectInputの初期化
	ComPtr<IDirectInput8> directInput = nullptr;
	//キーボードデバイスの生成
	ComPtr<IDirectInputDevice8> keyboard = nullptr;

private:
	KeyboardInput();

	~KeyboardInput();

public:
	//コピーコンストラクタを無効
	KeyboardInput(const KeyboardInput& obj) = delete;
	//代入演算子も
	KeyboardInput& operator=(const KeyboardInput& obj) = delete;


	static KeyboardInput& GetInstance();

	void Update();


	//トリガー用
	bool keyPush(BYTE keys)
	{
		return key[keys];
	}
	bool keyRelease(BYTE keys)
	{
		return !key[keys];
	}
	bool keyTrigger(BYTE keys)
	{
		return (key[keys] && !oldkey[keys]);
	}
	bool keyReleaseTrigger(BYTE keys)
	{
		return (!key[keys] && oldkey[keys]);
	}
};
#include "Input.h"
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

void Input::Initialize(WinApp* winApp) {
	HRESULT result;

	winApp_ = winApp;

	//DirectInputの初期化
	result = DirectInput8Create(
		winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//キーボードデバイスの生成
	result = directInput->CreateDevice(
		GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//入力データの形式のセット
	result = keyboard->SetDataFormat(
		&c_dfDIKeyboard);//標準形式
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::Update() {
	HRESULT result;

	//前回のキー入力を保存
	memcpy(keyPre_, key_, sizeof(key_));

	//キーボード情報の取得開始
	result = keyboard->Acquire();
	//全キーの入力状態を取得する
	result = keyboard->GetDeviceState(sizeof(key_), key_);
}

bool Input::ifKeyPress(BYTE keyNum) {
	//指定のキーを押していたら、trueを返す
	if (key_[keyNum]) {
		return true;
	}
	//基本はfalse
	return false;
}

bool Input::ifKeyTrigger(BYTE keyNum) {
	//1F前にキーを押していないことを確認
	if (!keyPre_[keyNum]) {
		//指定のキーを押していたら、trueを返す
		if (key_[keyNum]) {
			return true;
		}
	}
	//基本はfalse
	return false;
}

bool Input::ifKeyRelease(BYTE keyNum) {
	//1F前にキーを押されていることを確認
	if (keyPre_[keyNum]) {
		//指定のキーを押していなかったら、trueを返す
		if (!key_[keyNum]) {
			return true;
		}
	}
	//基本はfalse
	return false;
}
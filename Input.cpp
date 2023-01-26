#include "Input.h"
#include<wrl.h>


#include<cassert>
using namespace Microsoft::WRL;

void Input::Initialize(WindowsAPI* windowsApi)
{
	HRESULT result_;
	//借りてきたWinAppのインスタンスを記録
	this->windowsApi = windowsApi;

	//DirectInputのインスタンス生成
	result_ = DirectInput8Create(
		windowsApi->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result_));
	//キーボードデバイス生成
	result_ = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result_));
	//入力データ形式のセット
	result_ = keyboard->SetDataFormat(&c_dfDIKeyboard);//標準形式
	assert(SUCCEEDED(result_));
	//排他制御レベルのセット
	result_ = keyboard->SetCooperativeLevel(
		windowsApi->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result_));
}

void Input::Update()
{
	//キーボード情報の取得開始
	keyboard->Acquire();
	//前フレームのキー情報を保存
	for (int i = 0; i < 256; i++) {
		oldkey[i] = key[i];
	}
	//全キーの入力状態を取得する
	keyboard->GetDeviceState(sizeof(key), key);
}

bool Input::IsTrigger(BYTE key_)
{
	return (key[key_] && !oldkey[key_]);
}

bool Input::IsPress(BYTE key_)
{
	return key[key_];
}

bool Input::IsRelease(BYTE key_)
{
	return (!key[key_] && oldkey[key_]);
}

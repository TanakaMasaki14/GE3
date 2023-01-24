#include "KeyboardInput.h"

KeyboardInput::KeyboardInput()
{
	result = DirectInput8Create(
		WindowsApp::GetInstance().Getw().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//キーボードデバイスの生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	//排他制御レベルのリセット
	result = keyboard->SetCooperativeLevel(
		WindowsApp::GetInstance().Gethwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	//画面が手前にあるとき入力受付｜デバイスをこのアプリだけで専有しない｜Windowsキーを無効
	assert(SUCCEEDED(result));
}

KeyboardInput::~KeyboardInput()
{
}

KeyboardInput& KeyboardInput::GetInstance()
{
	static KeyboardInput inst; // private なコンストラクタを呼び出す。
	return inst;
}

void KeyboardInput::Update()
{
	//前回のキー情報
	memcpy(oldkey, key, sizeof(key));

	//キーボード情報の取得開始
	keyboard->Acquire();
	//全キーの入力情報を取得
	keyboard->GetDeviceState(sizeof(key), key);
}

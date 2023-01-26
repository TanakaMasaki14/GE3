#pragma once

#include"DirectX.h"
#include"windows.h"
#include"WindowsAPI.h"

#define DIRECTINPUT_VERSION 0x0800		//DirectInputのバージョン指定
#include<dinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#include<wrl.h>

class Input
{
public:

private:
	Microsoft::WRL::ComPtr<IDirectInput8> directInput = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard;
	BYTE key[256]{};
	BYTE oldkey[256]{};

	WindowsAPI* windowsApi = nullptr;
public:
	void Initialize(WindowsAPI* windowsApi);
	void Update();
	bool IsTrigger(BYTE key_);
	bool IsPress(BYTE key_);
	bool IsRelease(BYTE key_);
};


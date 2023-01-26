#include "Input.h"
#include<wrl.h>


#include<cassert>
using namespace Microsoft::WRL;

void Input::Initialize(WindowsAPI* windowsApi)
{
	HRESULT result_;
	//�؂�Ă���WinApp�̃C���X�^���X���L�^
	this->windowsApi = windowsApi;

	//DirectInput�̃C���X�^���X����
	result_ = DirectInput8Create(
		windowsApi->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result_));
	//�L�[�{�[�h�f�o�C�X����
	result_ = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result_));
	//���̓f�[�^�`���̃Z�b�g
	result_ = keyboard->SetDataFormat(&c_dfDIKeyboard);//�W���`��
	assert(SUCCEEDED(result_));
	//�r�����䃌�x���̃Z�b�g
	result_ = keyboard->SetCooperativeLevel(
		windowsApi->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result_));
}

void Input::Update()
{
	//�L�[�{�[�h���̎擾�J�n
	keyboard->Acquire();
	//�O�t���[���̃L�[����ۑ�
	for (int i = 0; i < 256; i++) {
		oldkey[i] = key[i];
	}
	//�S�L�[�̓��͏�Ԃ��擾����
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

#include "Input.h"
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

void Input::Initialize(WinApp* winApp) {
	HRESULT result;

	winApp_ = winApp;

	//DirectInput�̏�����
	result = DirectInput8Create(
		winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//�L�[�{�[�h�f�o�C�X�̐���
	result = directInput->CreateDevice(
		GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//���̓f�[�^�̌`���̃Z�b�g
	result = keyboard->SetDataFormat(
		&c_dfDIKeyboard);//�W���`��
	assert(SUCCEEDED(result));

	//�r�����䃌�x���̃Z�b�g
	result = keyboard->SetCooperativeLevel(
		winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::Update() {
	HRESULT result;

	//�O��̃L�[���͂�ۑ�
	memcpy(keyPre_, key_, sizeof(key_));

	//�L�[�{�[�h���̎擾�J�n
	result = keyboard->Acquire();
	//�S�L�[�̓��͏�Ԃ��擾����
	result = keyboard->GetDeviceState(sizeof(key_), key_);
}

bool Input::ifKeyPress(BYTE keyNum) {
	//�w��̃L�[�������Ă�����Atrue��Ԃ�
	if (key_[keyNum]) {
		return true;
	}
	//��{��false
	return false;
}

bool Input::ifKeyTrigger(BYTE keyNum) {
	//1F�O�ɃL�[�������Ă��Ȃ����Ƃ��m�F
	if (!keyPre_[keyNum]) {
		//�w��̃L�[�������Ă�����Atrue��Ԃ�
		if (key_[keyNum]) {
			return true;
		}
	}
	//��{��false
	return false;
}
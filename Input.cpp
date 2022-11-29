#include "Input.h"
#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

void Input::Initialize(HINSTANCE hInstance, HWND hwnd) {
	HRESULT result;

	//DirectInput�̏�����
	result = DirectInput8Create(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
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
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::Update() {
	HRESULT result;

	//�O��̃L�[���͂�ۑ�
	memcpy(keyPre, key, sizeof(key));

	//�L�[�{�[�h���̎擾�J�n
	result = keyboard->Acquire();
	//�S�L�[�̓��͏�Ԃ��擾����
	result = keyboard->GetDeviceState(sizeof(key), key);
}

bool Input::ifKeyPress(BYTE keyNum) {
	//�w��̃L�[�������Ă�����Atrue��Ԃ�
	if (key[keyNum]) {
		return true;
	}
	//��{��false
	return false;
}

bool Input::ifKeyTrigger(BYTE keyNum) {
	//1F�O�ɃL�[�������Ă��Ȃ����Ƃ��m�F
	if (!keyPre[keyNum]) {
		//�w��̃L�[�������Ă�����Atrue��Ԃ�
		if (key[keyNum]) {
			return true;
		}
	}
	//��{��false
	return false;
}
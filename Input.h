#pragma once
#include <wrl.h>

#include "WinApp.h"

#define DIRECT_VERSION 0x0800	//DirectInput�̃o�[�W�����w��
#include <dinput.h>

//����
class Input {
public: //namespace�̏ȗ�
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public: //��{�I�ȃ����o�֐�
	//������
	void Initialize(WinApp* winApp);
	//�X�V
	void Update();

public: //Input�ŗL�̃����o�֐�
	/// <summary>
	/// �L�[���������܂�Ă��邩���m�F
	/// </summary>
	/// <param name="keyNum">�L�[�̔ԍ�</param>
	/// <returns>�������܂�Ă��邩</returns>
	bool ifKeyPress(BYTE keyNum);

	/// <summary>
	/// �L�[���������ꂽ�����m�F
	/// </summary>
	/// <param name="keyNum">�L�[�̔ԍ�</param>
	/// <returns>�������ꂽ��</returns>
	bool ifKeyTrigger(BYTE keyNum);

	/// <summary>
	/// �L�[���������ꂽ�����m�F
	/// </summary>
	/// <param name="keyNum">�L�[�̔ԍ�</param>
	/// <returns>�������ꂽ��</returns>
	bool ifKeyRelease(BYTE keyNum);

private: //�悭�g�������o�ϐ�
	//DirectInput�̃C���X�^���X
	ComPtr<IDirectInput8> directInput = nullptr;
	//�L�[�{�[�h�f�o�C�X
	ComPtr<IDirectInputDevice8> keyboard = nullptr;

	//���͂��ꂽ�L�[�ԍ�
	BYTE key_[256] = {};
	BYTE keyPre_[256] = {};

	//WindowsAPI
	WinApp* winApp_ = nullptr;
};
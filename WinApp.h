#pragma once
#include <Windows.h>

class WinApp {
public: //�ÓI�����o�֐�(static)

	static LRESULT WindowProc(
		HWND hwnd,
		UINT msg,
		WPARAM wparam,
		LPARAM lparam);

public: //��{�I�ȃ����o�֐�

	//������
	void Initialize();
	//�X�V
	void Update();
	//�I��
	void Finalize();

public: //�ŗL�̃����o�֐�

	//�A�v���I�����b�Z�[�W�����邩
	bool ProcessMessage();

public: //�Q�b�^�[

	HINSTANCE GetHInstance() const { return w_.hInstance; }
	HWND GetHwnd() const {return hwnd_;}

public: //�O���Ŏg����萔

	//�T�C�Y
	static const int WinWidth = 1280;
	static const int WinHeight = 720;

private: //�悭�g�������o�ϐ�

	//�N���X�ݒ�
	WNDCLASSEX w_{};
	//�E�B���h�E�n���h��
	HWND hwnd_ = nullptr;

};
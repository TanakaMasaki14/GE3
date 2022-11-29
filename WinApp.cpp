#include "WinApp.h"

//�E�B���h�E�v���V�[�W��
LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
		//�E�B���h�E�j�����ꂽ�Ȃ�
	case WM_DESTROY:
		//OS�ɑ΂��ăA�v���I����ʒm
		PostQuitMessage(0);
		return 0;
	}

	//���b�Z�[�W����
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WinApp::Initialize(){
	//�N���X�ݒ�
	w_.cbSize = sizeof(WNDCLASSEX);
	w_.lpfnWndProc = (WNDPROC)WindowProc;
	w_.lpszClassName = L"DirectXGame";
	w_.hInstance = GetModuleHandle(nullptr);
	w_.hCursor = LoadCursor(NULL, IDC_ARROW);

	//OS�ɓo�^
	RegisterClassEx(&w_);
	//�T�C�Y
	RECT wrc = { 0,0,WinWidth,WinHeight };
	//�����ŃT�C�Y�C��
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//�E�B���h�E�n���h��
	hwnd_ = CreateWindow(w_.lpszClassName,
		L"LE2B_01_�A�J�C�P_�J�P��_GE3",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		w_.hInstance,
		nullptr);

	//�\����Ԃɂ���
	ShowWindow(hwnd_, SW_SHOW);
}

void WinApp::Update(){
}

void WinApp::Finalize(){
	//�N���X�o�^������
	UnregisterClass(w_.lpszClassName, w_.hInstance);
}

bool WinApp::ProcessMessage(){
	MSG msg{};

	//���b�Z�[�W�͂��邩�H
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//�~�ŏI�����b�Z�[�W�������烋�[�v�𔲂���
	if (msg.message == WM_QUIT) {
		return true;
	}

	return false;
}

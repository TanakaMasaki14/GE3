#include"Scene.h"
#include"DebugText.h"



//windows�A�v���ł̃G���g���[�|�C���g(main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//�R���\�[���ւ̕�������
	OutputDebugStringA("Hello,DirectX!!\n");

	//������
	WindowsApp::GetInstance();
	Directx::GetInstance();

	DrawInitialize();

	MSG msg{};	//���b�Z�[�W

	//�L�[�{�[�h���͏�����
	KeyboardInput::GetInstance();

	//�V�[��
	Scene* scene = new Scene();
	scene->Initialize();

	//�`�揉��������-------------

   //�����������@�����܂�//

   //�Q�[�����[�v
	while (true) {

		if (WindowsApp::GetInstance().MessegeRoop(msg))
		{
			break;
		}

		//���t���[�������@��������//
		//�L�[�{�[�h���̎擾�J�n
		KeyboardInput::GetInstance().Update();

		Directx::GetInstance().DrawUpdate({ 0.1f,0.25f, 0.5f,0.0f });

		//�X�V����
		scene->Update();


		// 4.�`��R�}���h��������@//-----------
		scene->Draw();

		// 4.�`��R�}���h�����܂� //

		Directx::GetInstance().DrawUpdate2();

		//���t���[�������@�����܂�//

		if (KeyboardInput::GetInstance().keyPush(DIK_ESCAPE)) break;
	}

	delete scene;


	//�E�B���h�E�N���X��o�^����
	WindowsApp::GetInstance().UnregisterClassA();

	return 0;
}



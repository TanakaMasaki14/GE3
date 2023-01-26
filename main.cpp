#include <DirectXMath.h>
using namespace DirectX;
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include"Input.h"
#include<DirectXTex.h>
//#include"WindowsAPI.h"
//#include"DirectX.h"
#include"Object3d.h"
#include"Util.h"
#include"Texture.h"
#include"GpPipeline.h"
#include"Gridline.h"
#include<string>
#include"SpriteManager.h"
#include"Sprite.h"
#include"Material.h"
using namespace Microsoft::WRL;
#include"Matrix4.h"
#include"Camera.h"
#include"ImguiManager.h"

#include"AudioManager.h"


//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̃Z�b�g
struct PipelineSet {
	//�p�C�v���C���X�e�[�g�I�u�W�F�N�g
	ComPtr<ID3D12PipelineState> pipelineState;
	//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootsignature;
};

void CreatepipeLine3D(ID3D12Device* dev);



//Windows�A�v���ł̃G���g���[�|�C���g(main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

#pragma region ��ՃV�X�e��������
	//windowsAPI����������
	WindowsAPI* windowsAPI = new WindowsAPI();
	windowsAPI->Initialize();

	// DirectX����������
	ReDirectX* directX = new ReDirectX();
	directX->Initialize(windowsAPI);

	HRESULT result{};

	//�L�[�{�[�h����������
	Input* input = new Input();
	input->Initialize(windowsAPI);

	//�e�N�X�`���}�l�[�W���[�̏�����
	Texture::Initialize(directX->GetDevice());

	SpriteManager* spriteManager = nullptr;
	//�X�v���C�g���ʕ��̏�����
	spriteManager = new SpriteManager;
	spriteManager->Initialize(directX, WindowsAPI::winW, WindowsAPI::winH);

	//3D�I�u�W�F�N�g�̏�����
	Object3d::StaticInitialize(directX);

	//�J�����N���X������
	Camera::StaticInitialize(directX->GetDevice());

	//imgui������
	ImguiManager* imguiManager = new ImguiManager();
	imguiManager->Initialize(windowsAPI, directX);

	//�I�[�f�B�I������
	AudioManager::StaticInitialize();
#pragma endregion ��ՃV�X�e��������

#pragma region �`�揉��������

	//�摜�ǂݍ���
	uint32_t marioGraph = Texture::LoadTexture(L"Resources/mario.jpg");
	uint32_t reimuGraph = Texture::LoadTexture(L"Resources/reimu.png");

	//�X�v���C�g�ꖇ�̏�����
	Sprite* sprite = new Sprite();
	sprite->Initialize(spriteManager, marioGraph);

	Sprite* sprite2 = new Sprite();
	sprite2->Initialize(spriteManager, reimuGraph);
	//sprite2->SetTextureNum(1);

	Model* skyDome;
	skyDome = Model::CreateModel("skydome");

	Object3d object1;
	object1.Initialize();
	object1.SetModel(skyDome);
	//object1.scale = XMFLOAT3(0.2f, 0.2f, 0.2f);
	object1.position = XMFLOAT3(0, 0, 50.0f);

	AudioManager newAudio;
	newAudio.SoundLoadWave("Resources/Alarm01.wav");


	//�����_���Ȑ��l���擾
	float randValue = Random(-100, 100);

	const size_t kObjCount = 50;
	Object3d obj[kObjCount];

	Object3d object;

	XMMATRIX matProjection;
	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, 0);	//���_���W
	XMFLOAT3 target(0, 0, 10);	//�����_���W
	XMFLOAT3 up(0, 1, 0);		//������x�N�g��

	Camera camera;
	camera.Initialize(eye, target, up);

	//�������f�Ԋҍs��̌v�Z
	//��p�̍s���錾
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),					//�㉺��p45�x
		(float)WindowsAPI::winW / WindowsAPI::winH,	//�A�X�y�N�g��i��ʉ���/��ʏc���j
		0.1f, 1000.0f								//�O���A����
	);
	//�r���[�ϊ��s��̌v�Z
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	//	ImGui::Text("Hello,world %d", 123);
		/*if (ImGui::Button("Save")) {
			M
		}*/

	XMFLOAT2 spritePos(0, 0);

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


	bool isPlayAudio = false;

#pragma endregion �`�揉��������
	// �Q�[�����[�v
	while (true) {

#pragma region ��ՃV�X�e��������
		//windows�̃��b�Z�[�W����
		if (windowsAPI->ProcessMessage()) {
			//���[�v�𔲂���
			break;
		}
		input->Update();
#pragma endregion ��ՃV�X�e��������
#pragma region �V�[���X�V����

		imguiManager->Begin();



		//ImGui�e�X�g�E�B���h�E
		{
			static float f = 0.0f;
			static int counter = 0;


			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();

			ImGui::ShowDemoWindow();

		}

		//�X�v���C�g���W
		ImGui::Begin("bebug1");
		ImGui::SliderFloat("positionX", &spritePos.x, 0.0f, static_cast<float>(WindowsAPI::winW), "%4.1f");
		ImGui::SliderFloat("positionY", &spritePos.y, 0.0f, static_cast<float>(WindowsAPI::winH), "%4.1f");
		ImGui::Checkbox("audio", &isPlayAudio);
		ImGui::End();


		if (isPlayAudio) {
			newAudio.SoundPlayWave();
		}
		else {
			newAudio.StopWave();
		}

		sprite->SetPos(spritePos);
		sprite2->SetPos({ WindowsAPI::winW / 2,WindowsAPI::winH / 2 });
		sprite->SetSize({ 64,64 });

		if (input->IsPress(DIK_A)) {
			object1.rotation.y += 0.1f;
		}
		else if (input->IsPress(DIK_D)) {
			object1.rotation.y -= 0.1f;
		}
		if (input->IsPress(DIK_W)) {
			object1.rotation.z += 0.1f;
		}
		else if (input->IsPress(DIK_S)) {
			object1.rotation.z -= 0.1f;
		}

		//object1.scale = { 50,50,50 };


		object1.Update();

#pragma endregion �V�[���X�V����

		imguiManager->End();

		//�`��O����
		directX->BeginDraw();
#pragma region �V�[���`�揈��

		//3D�I�u�W�F�N�g�`�揈��
		Object3d::BeginDraw(camera);
		object1.Draw();

		//�X�v���C�g�`�揈��
		spriteManager->beginDraw();
		sprite->Draw();
		//sprite2->Draw();


		//ImGui�`�揈��
		imguiManager->Draw();

#pragma endregion �V�[���`�揈��
		// �S�D�`��R�}���h�����܂�
		directX->EndDraw();
		// DirectX���t���[������ �����܂�
	}
#pragma region �V�[���I������

	//ImGui�I������
	imguiManager->Finalize();
	delete imguiManager;

	//WindowsAPI�I������
	windowsAPI->Finalize();

	delete sprite;

	delete windowsAPI;
	delete input;
	delete directX;
	delete spriteManager;

	delete skyDome;

#pragma endregion �V�[���I������

	return 0;
}

void MatrixUpdate()
{
}

void CreatepipeLine3D(ID3D12Device* dev)
{


}

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


//パイプラインステートとルートシグネチャのセット
struct PipelineSet {
	//パイプラインステートオブジェクト
	ComPtr<ID3D12PipelineState> pipelineState;
	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootsignature;
};

void CreatepipeLine3D(ID3D12Device* dev);



//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

#pragma region 基盤システム初期化
	//windowsAPI初期化処理
	WindowsAPI* windowsAPI = new WindowsAPI();
	windowsAPI->Initialize();

	// DirectX初期化処理
	ReDirectX* directX = new ReDirectX();
	directX->Initialize(windowsAPI);

	HRESULT result{};

	//キーボード初期化処理
	Input* input = new Input();
	input->Initialize(windowsAPI);

	//テクスチャマネージャーの初期化
	Texture::Initialize(directX->GetDevice());

	SpriteManager* spriteManager = nullptr;
	//スプライト共通部の初期化
	spriteManager = new SpriteManager;
	spriteManager->Initialize(directX, WindowsAPI::winW, WindowsAPI::winH);

	//3Dオブジェクトの初期化
	Object3d::StaticInitialize(directX);

	//カメラクラス初期化
	Camera::StaticInitialize(directX->GetDevice());

	//imgui初期化
	ImguiManager* imguiManager = new ImguiManager();
	imguiManager->Initialize(windowsAPI, directX);

	//オーディオ初期化
	AudioManager::StaticInitialize();
#pragma endregion 基盤システム初期化

#pragma region 描画初期化処理

	//画像読み込み
	uint32_t marioGraph = Texture::LoadTexture(L"Resources/mario.jpg");
	uint32_t reimuGraph = Texture::LoadTexture(L"Resources/reimu.png");

	//スプライト一枚の初期化
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


	//ランダムな数値を取得
	float randValue = Random(-100, 100);

	const size_t kObjCount = 50;
	Object3d obj[kObjCount];

	Object3d object;

	XMMATRIX matProjection;
	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, 0);	//視点座標
	XMFLOAT3 target(0, 0, 10);	//注視点座標
	XMFLOAT3 up(0, 1, 0);		//上方向ベクトル

	Camera camera;
	camera.Initialize(eye, target, up);

	//透視東映返還行列の計算
	//専用の行列を宣言
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),					//上下画角45度
		(float)WindowsAPI::winW / WindowsAPI::winH,	//アスペクト比（画面横幅/画面縦幅）
		0.1f, 1000.0f								//前橋、奥橋
	);
	//ビュー変換行列の計算
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

#pragma endregion 描画初期化処理
	// ゲームループ
	while (true) {

#pragma region 基盤システム初期化
		//windowsのメッセージ処理
		if (windowsAPI->ProcessMessage()) {
			//ループを抜ける
			break;
		}
		input->Update();
#pragma endregion 基盤システム初期化
#pragma region シーン更新処理

		imguiManager->Begin();



		//ImGuiテストウィンドウ
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

		//スプライト座標
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

#pragma endregion シーン更新処理

		imguiManager->End();

		//描画前処理
		directX->BeginDraw();
#pragma region シーン描画処理

		//3Dオブジェクト描画処理
		Object3d::BeginDraw(camera);
		object1.Draw();

		//スプライト描画処理
		spriteManager->beginDraw();
		sprite->Draw();
		//sprite2->Draw();


		//ImGui描画処理
		imguiManager->Draw();

#pragma endregion シーン描画処理
		// ４．描画コマンドここまで
		directX->EndDraw();
		// DirectX毎フレーム処理 ここまで
	}
#pragma region シーン終了処理

	//ImGui終了処理
	imguiManager->Finalize();
	delete imguiManager;

	//WindowsAPI終了処理
	windowsAPI->Finalize();

	delete sprite;

	delete windowsAPI;
	delete input;
	delete directX;
	delete spriteManager;

	delete skyDome;

#pragma endregion シーン終了処理

	return 0;
}

void MatrixUpdate()
{
}

void CreatepipeLine3D(ID3D12Device* dev)
{


}

#include "Scene.h"
#include <sstream>
#include <iomanip>

void SceneState::SetScene(Scene* scene)
{
	//stateではなくSceneクラスのインスタンス//
	this->scene = scene;
}



//---------------------------------------------------------------------------------------
//タイトル
void SceneTitle::Initialize()
{

}

void SceneTitle::Update()
{
	/*if (1){scene->ChangeState(new SceneGame);}*/
}

void SceneTitle::Draw()
{

}



//---------------------------------------------------------------------------------------
//ゲーム
void SceneGame::Initialize()
{

}

void SceneGame::Update()
{
	//シーン遷移
	//if (1) scene->ChangeState(new SceneEnd);
}

void SceneGame::Draw()
{

}



//---------------------------------------------------------------------------------------
//終了画面
void SceneEnd::Initialize()
{

}

void SceneEnd::Update()
{
	//if (1) scene->ChangeState(new SceneTitle);
}

void SceneEnd::Draw()
{

}



//---------------------------------------------------------------------------------------
//常に行うもの
Scene::~Scene()
{
	delete state;
	for (int i = 0; i < _countof(model); i++)
	{
		delete model[i];
	}
	imGuiManager->Finalize();
	delete imGuiManager;
	delete lightManager;
	//音データ解放

}

void Scene::ChangeState(SceneState* state)
{
	delete this->state;
	this->state = state;
	state->SetScene(this);
	state->Initialize();
}

void Scene::Initialize()
{
	//音
	Sound::LoadWave("a.wav", true);
	Sound::LoadWave("BGM.wav", false);//

	Sound::PlayWave("BGM.wav", 5.0f, true);

	//画像
	TextureManager::LoadGraph(L"Resources/ascii.png", debugTextHandle);
	TextureManager::LoadGraph(L"Resources/image/white.png", texhandle[0]);
	TextureManager::LoadGraph(L"Resources/image/particle.png", texhandle[1]);
	TextureManager::LoadGraph(L"Resources/image/p.jpg", texhandle[2]);

	model[0] = Model::LoadFromOBJ("skydome");
	draw[0].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	model[1] = Model::LoadFromOBJ("ground");
	draw[1].worldMat->scale = { 10.0f, 10.0f, 10.0f };
	draw[1].worldMat->trans = { 10.0f, -5.0f, 0 };
	model[2] = Model::LoadFromOBJ("chr_sword");
	draw[2].worldMat->scale = { 10,10,10 };
	draw[2].worldMat->trans = { 5.0f, -4.0f, 0 };
	model[3] = Model::LoadFromOBJ("sphere", true);
	draw[3].worldMat->scale = { 5,5,5 };
	model[4] = Model::LoadFromOBJ("sphere");
	draw[4].worldMat->scale = { 4,4,4 };
	draw[4].worldMat->trans = { -10.0f, 0.0f, 0 };

	draw[3].worldMat->trans.x += 30.0f;
	draw[6].worldMat->trans.x -= 30.0f;
	draw[7].worldMat->trans.z -= 20.0f;

	//imgui
	imGuiManager = new ImGuiManager();
	imGuiManager->Initialize();

	ChangeState(new SceneTitle);
	state->SetScene(this);

	//Light
	LightManager::StaticInitialize();
	//インスタンス生成
	lightManager = LightManager::Create();
	//ライト色を設定
	lightManager->SetDirLightColor(0, { 1,1,1 });
	//3Dオブジェクトにライトをセット(全体で一つを共有)
	Draw::SetLight(lightManager);

	//点光源
	lightManager->SetDirLightActive(0, true);
	lightManager->SetDirLightActive(1, false);
	lightManager->SetDirLightActive(2, false);
	lightManager->SetPointLightActive(0, true);
	pointLightPos[0] = 0.5f;
	pointLightPos[1] = 1.0f;
	pointLightPos[2] = 0.0f;
}

void Scene::Update()
{
	//imgui
	imGuiManager->Begin();

	{
		//デモ
		ImGui::ShowDemoWindow();

		{
			//ライト
			////光線方向初期化
			//static XMVECTOR lightDir = { 0,1,5,0 };

			//if (KeyboardInput::GetInstance().keyPush(DIK_UP)) { lightDir.m128_f32[1] += 1.0f; }
			//if (KeyboardInput::GetInstance().keyPush(DIK_DOWN)) { lightDir.m128_f32[1] -= 1.0f; }
			//if (KeyboardInput::GetInstance().keyPush(DIK_RIGHT)) { lightDir.m128_f32[0] += 1.0f; }
			//if (KeyboardInput::GetInstance().keyPush(DIK_LEFT)) { lightDir.m128_f32[0] -= 1.0f; }

			//light->SetLightDir(lightDir);

			//std::ostringstream debugstr;
			//debugstr << "lightDirFactor("
			//	<< std::fixed << std::setprecision(2)
			//	<< lightDir.m128_f32[0] << ","
			//	<< lightDir.m128_f32[1] << ","
			//	<< lightDir.m128_f32[2] << ")";
			//debugText.Print(debugstr.str(), 50, 50, 1.0f);

			//debugstr.str("");
			//debugstr.clear();

			//const XMFLOAT3& cameraPos = { viewMat.eye.x,viewMat.eye.y,viewMat.eye.z };
			//debugstr << "cameraPos("
			//	<< std::fixed << std::setprecision(2)
			//	<< lightDir.m128_f32[0] << ","
			//	<< lightDir.m128_f32[1] << ","
			//	<< lightDir.m128_f32[2] << ")";
			//debugText.Print(debugstr.str(), 50, 70, 1.0f);


			//点光源
			lightManager->SetPointLightPos(0, XMFLOAT3(pointLightPos));
			lightManager->SetPointLightColor(0, XMFLOAT3(pointLightColor));
			lightManager->SetPointLightAtten(0, XMFLOAT3(pointLightAtten));

			static bool a = true;
			ImGui::Begin("PointLight", &a, ImGuiWindowFlags_MenuBar);
			ImGui::ColorEdit3("pointLightColor", pointLightColor, ImGuiColorEditFlags_Float);
			ImGui::InputFloat3("pointLightPos", pointLightPos);
			ImGui::InputFloat3("pointLight", pointLightAtten);
			ImGui::End();
			lightManager->Update();
		}
	}

	state->Update();

	//move
	{
		draw[2].worldMat->rot.y = draw[2].worldMat->rot.y + 0.01f;
		draw[3].worldMat->rot.y = draw[3].worldMat->rot.y + 0.01f;
		draw[4].worldMat->rot.y = draw[4].worldMat->rot.y + 0.01f;
		draw[6].worldMat->rot.y = draw[6].worldMat->rot.y + 0.01f;
		draw[2].worldMat->SetWorld();
		draw[3].worldMat->SetWorld();
		draw[4].worldMat->SetWorld();
		draw[6].worldMat->SetWorld();
	}

	{
		cameraWorldMat.rot.y += (KeyboardInput::GetInstance().keyPush(DIK_A) - KeyboardInput::GetInstance().keyPush(DIK_D)) * 0.05f;

		cameraWorldMat.rot.x += (KeyboardInput::GetInstance().keyPush(DIK_W) - KeyboardInput::GetInstance().keyPush(DIK_S)) * 0.05f;
		cameraWorldMat.rot.x = min(cameraWorldMat.rot.x, pi / 2.0f);
		cameraWorldMat.rot.x = max(cameraWorldMat.rot.x, -pi / 2.0f);

		cameraWorldMat.SetWorld();
		viewMat.eye = cameraPos;
		Vec3xM4(viewMat.eye, cameraWorldMat.matWorld, 0);
		viewMat.SetMat();
	}

	if (KeyboardInput::GetInstance().keyTrigger(DIK_SPACE))
	{
		Sound::PlayWave("a.wav");
		Sound::PlayWave("BGM.wav");
	}
	if (KeyboardInput::GetInstance().keyTrigger(DIK_1))
	{
		Sound::StopWave("a.wav");
		Sound::StopWave("BGM.wav");
	}


#ifdef _DEBUG
	if (KeyboardInput::GetInstance().keyTrigger(DIK_E)) ChangeState(new SceneTitle);
#endif 

	//imgui
	imGuiManager->End();
}

void Scene::Draw()
{
	draw[0].DrawModel(draw[0].worldMat, &viewMat, &projectionMat, model[0]);
	draw[1].DrawModel(draw[1].worldMat, &viewMat, &projectionMat, model[1]);
	draw[2].DrawModel(draw[2].worldMat, &viewMat, &projectionMat, model[2]);
	draw[3].DrawModel(draw[3].worldMat, &viewMat, &projectionMat, model[3]);
	draw[4].DrawModel(draw[4].worldMat, &viewMat, &projectionMat, model[4]);

	state->Draw();

	//スプライト
	//draw[5].DrawBoxSprite({ pos[0],pos[1],0 }, 1.0f, { 1.0f,1.0f,1.0f,1.0f }, texhandle[2]);
	/*draw[8].DrawBoxSprite({ 100,500,0 }, scale, { 1.0f,1.0f,0,1.0f }, texhandle[2], { 0.5f,0.5f }, false);
	draw[9].DrawBoxSprite({ 100,500,0 }, scale, { 1.0f,1.0f,0,1.0f }, texhandle[2], { 0.0f,0.0f },false,true);*/

	draw[6].DrawCube3D(draw[6].worldMat, &viewMat, &projectionMat, { 1,1,1,1 }, texhandle[0]);

	debugText.DrawAll(debugTextHandle);

	//imgui
	imGuiManager->Draw();
}


#include "LightManager.h"

ID3D12Device* LightManager::device_ = nullptr;


void LightManager::StaticInitialize()
{
	//再初期化をチェック
	assert(!LightManager::device_);
	//nullptrチェック
	assert(Directx::GetInstance().GetDevice());
	//セット
	LightManager::device_ = Directx::GetInstance().GetDevice();
}

void LightManager::Initialize()
{
	DefaultLightSetting();

	//ヒープ設定
	UINT sizeVB;
	D3D12_RESOURCE_DESC resDesc{}; D3D12_HEAP_PROPERTIES heapProp{};
	// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
	sizeVB = static_cast<UINT>(sizeof(ConstBufferData));
	//頂点バッファの設定		//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

	ResourceProperties(resDesc, sizeVB);
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	//定数バッファの生成
	BuffProperties(heapProp, resDesc, constBuff.GetAddressOf());

	//定数バッファへデータ転送
	TransferConstBuffer();

	//点光源
	PointLight::ConstBufferData pointLights[PointLightNum];
}

void LightManager::TransferConstBuffer()
{
	HRESULT result;
	//定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		//環境光
		constMap->ambientColor = ambientColor;

		for (int i = 0; i < DirLightNum; i++) {

			//平行光源
			// ライトが有効なら設定を転送
			if (dirLights[i].IsActive()) {
				constMap->dirLights[i].active = 1;
				constMap->dirLights[i].lightv = -dirLights[i].GetLightDir();
				constMap->dirLights[i].lightColor = dirLights[i].GetLightColor();
			}
			// ライトが無効ならライト色を0に
			else {
				constMap->dirLights[i].active = 0;
			}

			//点光源
			//ライトが有効なら設定を転送
			if (pointLights[i].GetActive())
			{
				constMap->pointLights[i].active = 1;
				constMap->pointLights[i].lightpos = pointLights[i].GetLightPos();
				constMap->pointLights[i].lightcolor =
					pointLights[i].GetLightColor();
				constMap->pointLights[i].lightatten =
					pointLights[i].GetLightAtten();
			}
			//ライトが無効ならライト色を0に
			else
			{
				constMap->pointLights[i].active = 0;
			}
		}

		constBuff->Unmap(0, nullptr);
	}
}

void LightManager::DefaultLightSetting()
{
	dirLights[0].SetActive(true);
	dirLights[0].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights[0].SetLightDir({ 0.0f, -1.0f, 0.0f, 0 });

	dirLights[1].SetActive(true);
	dirLights[1].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights[1].SetLightDir({ +0.5f, +0.1f, +0.2f, 0 });

	dirLights[2].SetActive(true);
	dirLights[2].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights[2].SetLightDir({ -0.5f, +0.1f, -0.2f, 0 });
}

void LightManager::SetAmbientColor(const XMFLOAT3& color)
{
	ambientColor = color;
	dirty = true;
}

void LightManager::Update()
{
	//値の更新があった時だけ定数バッファに転送
	if (dirty)
	{
		TransferConstBuffer();
		dirty = false;
	}
}

void LightManager::Draw(UINT rootParamaterIndex)
{
	//定数バッファビューをセット
	Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(
		rootParamaterIndex, constBuff->GetGPUVirtualAddress()
	);
}

LightManager* LightManager::Create()
{
	//3Dオブジェクトのインスタンスを生成
	LightManager* instance = new LightManager();
	//初期化
	instance->Initialize();
	//生成したインスタンスを返す
	return instance;
}

//-----------------------------------------------------------
void LightManager::SetDirLightActive(int index, bool active)
{
	assert(0 <= index && index < DirLightNum);

	dirLights[index].SetActive(active);
}

void LightManager::SetDirLightDir(int index, const XMVECTOR& lightdir)
{
	assert(0 <= index && index < DirLightNum);

	dirLights[index].SetLightDir(lightdir);
	dirty = true;
}

void LightManager::SetDirLightColor(int index, const XMFLOAT3& lightcolor)
{
	assert(0 <= index && index < DirLightNum);

	dirLights[index].SetLightColor(lightcolor);
	dirty = true;
}


//-------------------------------------------------------------
void LightManager::SetPointLightActive(int index, bool active)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetActive(active);
}

void LightManager::SetPointLightPos(int index, const XMFLOAT3& pos)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetLightPos(pos);

	dirty = true;
}

void LightManager::SetPointLightColor(int index, const XMFLOAT3& color)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetLightColor(color);

	dirty = true;
}

void LightManager::SetPointLightAtten(int index, const XMFLOAT3& atten)
{
	assert(0 <= index && index < PointLightNum);
	pointLights[index].SetLightAtten(atten);

	dirty = true;
}

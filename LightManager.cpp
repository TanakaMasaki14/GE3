#include "LightManager.h"

ID3D12Device* LightManager::device_ = nullptr;


void LightManager::StaticInitialize()
{
	//�ď��������`�F�b�N
	assert(!LightManager::device_);
	//nullptr�`�F�b�N
	assert(Directx::GetInstance().GetDevice());
	//�Z�b�g
	LightManager::device_ = Directx::GetInstance().GetDevice();
}

void LightManager::Initialize()
{
	DefaultLightSetting();

	//�q�[�v�ݒ�
	UINT sizeVB;
	D3D12_RESOURCE_DESC resDesc{}; D3D12_HEAP_PROPERTIES heapProp{};
	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^1���̃T�C�Y * ���_�f�[�^�̗v�f��
	sizeVB = static_cast<UINT>(sizeof(ConstBufferData));
	//���_�o�b�t�@�̐ݒ�		//�q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPU�ւ̓]���p

	ResourceProperties(resDesc, sizeVB);
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	//�萔�o�b�t�@�̐���
	BuffProperties(heapProp, resDesc, constBuff.GetAddressOf());

	//�萔�o�b�t�@�փf�[�^�]��
	TransferConstBuffer();

	//�_����
	PointLight::ConstBufferData pointLights[PointLightNum];
}

void LightManager::TransferConstBuffer()
{
	HRESULT result;
	//�萔�o�b�t�@�փf�[�^�]��
	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		//����
		constMap->ambientColor = ambientColor;

		for (int i = 0; i < DirLightNum; i++) {

			//���s����
			// ���C�g���L���Ȃ�ݒ��]��
			if (dirLights[i].IsActive()) {
				constMap->dirLights[i].active = 1;
				constMap->dirLights[i].lightv = -dirLights[i].GetLightDir();
				constMap->dirLights[i].lightColor = dirLights[i].GetLightColor();
			}
			// ���C�g�������Ȃ烉�C�g�F��0��
			else {
				constMap->dirLights[i].active = 0;
			}

			//�_����
			//���C�g���L���Ȃ�ݒ��]��
			if (pointLights[i].GetActive())
			{
				constMap->pointLights[i].active = 1;
				constMap->pointLights[i].lightpos = pointLights[i].GetLightPos();
				constMap->pointLights[i].lightcolor =
					pointLights[i].GetLightColor();
				constMap->pointLights[i].lightatten =
					pointLights[i].GetLightAtten();
			}
			//���C�g�������Ȃ烉�C�g�F��0��
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
	//�l�̍X�V���������������萔�o�b�t�@�ɓ]��
	if (dirty)
	{
		TransferConstBuffer();
		dirty = false;
	}
}

void LightManager::Draw(UINT rootParamaterIndex)
{
	//�萔�o�b�t�@�r���[���Z�b�g
	Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(
		rootParamaterIndex, constBuff->GetGPUVirtualAddress()
	);
}

LightManager* LightManager::Create()
{
	//3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	LightManager* instance = new LightManager();
	//������
	instance->Initialize();
	//���������C���X�^���X��Ԃ�
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
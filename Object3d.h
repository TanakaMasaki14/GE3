#pragma once

#include<DirectXMath.h>
#include<d3d12.h>
#include<wrl.h>
#include"Transform.h"
#include"DirectX.h"
#include<vector>
#include<string>
#include"Model.h"
#include"Camera.h"

class Object3d
{
public:

	//�T�u�N���X
		//���_�f�[�^�\����
	struct Vertex {
		DirectX::XMFLOAT3 pos;//xyz���W
		DirectX::XMFLOAT3 normal;//�@���x�N�g��
		DirectX::XMFLOAT2 uv;//uv���W
	};

	struct ConstBufferData {
		//DirectX::XMFLOAT4 color;
		DirectX::XMMATRIX mat;
	};


	static Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	static ReDirectX* directX;

	//���_���W���
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffB0;	//�萔�o�b�t�@�}�b�v�i�s��p�j

	ConstBufferData* constMap = nullptr;

	DirectX::XMFLOAT4 color = { 1,1,1,1 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };	//�A�t�B���ϊ����
	DirectX::XMFLOAT3 rotation = { 0,0,0 };	//�A�t�B���ϊ����
	DirectX::XMFLOAT3 position = { 0,0,0 };	//�A�t�B���ϊ����
	DirectX::XMMATRIX matWorld;	//���[���h�ϊ��s��
	Object3d* parent = nullptr;	//�e�I�u�W�F�N�g�ւ̃|�C���^

	Model* model = nullptr;	//���f���f�[�^

public:
	Object3d();
	~Object3d();

	//�ÓI�����o�֐�
	static void StaticInitialize(ReDirectX* directX_);
	static void BeginDraw(const Camera& camera);

	void Initialize();
	void Update();
	void Draw();
	void SetModel(Model* model_) { model = model_; }

private:
	static void CreatePipeline3D();
};


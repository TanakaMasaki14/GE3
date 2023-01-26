#include "Object3d.h"

#include"WindowsAPI.h"
#include<cassert>
using namespace DirectX;
using namespace Microsoft::WRL;
#include<d3dx12.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include"GpPipeline.h"
#include"Texture.h"
#include<fstream>
#include<sstream>
#include"Model.h"
#include<vector>
using namespace std;

//�ÓI�����o�ϐ�
ComPtr<ID3D12PipelineState> Object3d::pipelineState;
ComPtr<ID3D12RootSignature> Object3d::rootSignature;
ReDirectX* Object3d::directX = nullptr;


Object3d::Object3d()
{
}

Object3d::~Object3d()
{
}

void Object3d::StaticInitialize(ReDirectX* directX_)
{
	//null�`�F�b�N
	assert(directX_);
	//�����o�ɓn��
	directX = directX_;

	//���f���N���X�Ƀf�o�C�X�̃C���X�^���X��n��
	Model::SetDevice(directX->GetDevice());
	//3D�p�p�C�v���C���X�e�[�g����
	CreatePipeline3D();
}

void Object3d::BeginDraw(const Camera& camera)
{
	//�p�C�v���C���X�e�[�g�̐ݒ�
	directX->GetCommandList()->SetPipelineState(pipelineState.Get());
	//���[�g�V�O�l�`���̐ݒ�
	directX->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	//�v���~�e�B�u�`��̐ݒ�
	directX->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//3�Ԓ萔�o�b�t�@�r���[�ɃJ�����̒萔�o�b�t�@��ݒ�
	directX->GetCommandList()->SetGraphicsRootConstantBufferView(3, camera.constBuff->GetGPUVirtualAddress());
}

void Object3d::Initialize()
{
	ID3D12Device* device = directX->GetDevice();

	HRESULT result;
	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{  };
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferData) + 0xff) & ~0xff;	//256�o�C�g�A���C�������g
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//�萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&cbHeapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffB0));
	assert(SUCCEEDED(result));
	//�萔�o�b�t�@�̃}�b�s���O
	result = constBuffB0->Map(0, nullptr, (void**)&constMap);//�}�b�s���O
	assert(SUCCEEDED(result));


	XMMATRIX matrix = XMMatrixIdentity();
	constMap->mat = matrix;


}

void Object3d::Update()
{
	XMMATRIX matScale, matRot, matTrans;

	//�e�s��v�Z
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(rotation.z);	//Z������ɉ�]
	matRot *= XMMatrixRotationX(rotation.x);	//X������ɉ�]
	matRot *= XMMatrixRotationY(rotation.y);	//Y������ɉ�]
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	matWorld = XMMatrixIdentity();
	matWorld = XMMatrixIdentity();

	matWorld *= matScale;	//�X�P�[�����O�𔽉f
	matWorld *= matRot;	//��]�𔽉f
	matWorld *= matTrans;	//���s�ړ��𔽉f

	if (parent != nullptr) {
		matWorld *= parent->matWorld;
	}

	//�萔�o�b�t�@�ɓ]��
	//constMap->mat = matWorld * matView * matProjection;
	constMap->mat = matWorld;
}

void Object3d::Draw()
{
	ID3D12GraphicsCommandList* commandList = directX->GetCommandList();
	//h�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	commandList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	//���f���f�[�^�̕`��p�R�}���h�̂܂Ƃ܂�
	model->Draw(commandList, 2);
}

void Object3d::CreatePipeline3D()
{
	HRESULT result;

	ID3D12Device* dev = directX->GetDevice();

	ComPtr<ID3DBlob> vsBlob;		// ���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob;		// �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob;	// �G���[�I�u�W�F�N�g

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/ObjVertexShader.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);
	// �G���[�Ȃ�
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}


	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/ObjPixelShader.hlsl", // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);
	// �G���[�Ȃ�
	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//�p�C�v���C���ݒ�
	GpPipeline pipeline3D;
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
	inputLayout.push_back(
		{
		"POSITION",										//�Z�}���e�B�b�N��
		0,												//�����Z�}���e�B�b�N�����������鎞�Ɏg���C���f�b�N�X�i0�ł悢�j
		DXGI_FORMAT_R32G32B32_FLOAT,					//�v�f���ƃr�b�g����\���iXYZ��3��float�^�Ȃ̂�R32G32B32_FLOAT�j
		0,												//���̓X���b�g�C���f�b�N�X(0�ł悢)
		D3D12_APPEND_ALIGNED_ELEMENT,					//�f�[�^�̃I�t�Z�b�g�l(D3D12_APPEND_ALIGNED_ELEMENT���Ǝ����ݒ�)
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		//���̓f�[�^���(�W����D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA)
		0												//��x�ɕ`�悷��C���X�^���X��(0�ł悢)
		});

	inputLayout.push_back(
		{//�@���x�N�g��
		"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		});
	inputLayout.push_back({//uv���W
		"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		});

	pipeline3D.SetPipeline(vsBlob.Get(), psBlob.Get(), inputLayout);

	//�f�X�N���v�^�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;		//��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;	//�e�N�X�`�����W�X�^0��
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER rootParams[4] = {};
	//�萔�o�b�t�@0��
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//�萔�o�b�t�@�r���[
	rootParams[0].Descriptor.ShaderRegister = 0;					//�萔�o�b�t�@�ԍ�
	rootParams[0].Descriptor.RegisterSpace = 0;						//�f�t�H���g�l
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//���ׂẴV�F�[�_���猩����
	//�e�N�X�`�����W�X�^0��
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//���
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;			//�f�X�N���v�^�����W
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;						//�f�X�N���v�^�����W��
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//���ׂẴV�F�[�_���猩����
	//�萔�o�b�t�@1��
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//���
	rootParams[2].Descriptor.ShaderRegister = 1;					//�f�X�N���v�^�����W
	rootParams[2].Descriptor.RegisterSpace = 0;						//�f�X�N���v�^�����W��
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//���ׂẴV�F�[�_���猩����o�b�t�@
	//�萔�o�b�t�@2��
	rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//���
	rootParams[3].Descriptor.ShaderRegister = 2;					//�f�X�N���v�^�����W
	rootParams[3].Descriptor.RegisterSpace = 0;						//�f�X�N���v�^�����W��
	rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//���ׂẴV�F�[�_���猩����o�b�t�@

	//�e�N�X�`���T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;				//���J��Ԃ��i�^�C�����O�j
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;				//�c�J��Ԃ��i�^�C�����O�j
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;				//���s�J��Ԃ��i�^�C�����O�j
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	//�{�[�_�[�̎��͍�
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;					//���ׂă��j�A���
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;									//�~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f;												//�~�b�v�}�b�v�ŏ��l
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;			//�s�N�Z���V�F�[�_����̂ݎg�p�\

	// ���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;	//���[�g�p�����[�^�̐擪�A�h���X
	rootSignatureDesc.NumParameters = _countof(rootParams);		//���[�g�p�����[�^��
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;
	// ���[�g�V�O�l�`���̃V���A���C�Y
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));

	//�p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipeline3D.desc.pRootSignature = rootSignature.Get();

	//�p�C�v���C���X�e�[�g�̐���
	pipeline3D.SetPipelineState(dev, pipelineState);
}




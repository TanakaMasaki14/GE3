#pragma once
#include<d3d12.h>
#include"DirectX.h"
#include<DirectXMath.h>
#include"Texture.h"



struct VertexPosUv {
	DirectX::XMFLOAT3 pos;	//xyz���W
	DirectX::XMFLOAT2 uv;	//uv���W
};

struct ConstBufferData {
	DirectX::XMFLOAT4 color;
	DirectX::XMMATRIX mat;
};

class SpriteManager
{


public:

	static std::string defaultTextureDirectoryPath;
	ReDirectX* directX = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;		//�p�C�v���C���X�e�[�g
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;		//���[�g�V�O�l�`��

	DirectX::XMMATRIX matProjection;//�ˉe�s��


public:
	//������
	void Initialize(ReDirectX* directX, int windowWidth, int windowHeight);

	//�`��O����
	void beginDraw();

	/// <summary>
	/// �e�N�X�`���R�}���h�̔��s
	/// </summary>
	/// <param name="dev"></param>
	void SetTextureCommand(uint32_t index);

	//�w��ԍ��̃e�N�X�`���o�b�t�@���擾
	ID3D12Resource* GetTextureBuffer(uint32_t index)const { return Texture::texBuffuers[index].Get(); }

private:
	//�X�v���C�g�p�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐���
	void CreatePipeline2D(ID3D12Device* dev);
};


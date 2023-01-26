#pragma once
#include "Vector3.h"
#include"Vertex.h"
#include<vector>
#include"DirectX.h"
#include<wrl.h>
#include<DirectXMath.h>
#include"Transform.h"


class Gridline
{
public:
	Vector3 start{};
	Vector3 end{};
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffTransform;	//�萔�o�b�t�@�}�b�v�i�s��p�j
	ConstBufferDataTransform* constMapTransform = nullptr;	//�萔�o�b�t�@�}�b�v�i�s��p�j

	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;

	std::vector<Vertex> vertices;	//���_�f�[�^

	D3D12_VERTEX_BUFFER_VIEW vbview{};	//���_�o�b�t�@�r���[

	void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, int lineNum, D3D12_CPU_DESCRIPTOR_HANDLE& srvHandle);

	void Draw(ID3D12GraphicsCommandList* commandList, ID3D12DescriptorHeap* srvheaps);

	void Update(DirectX::XMMATRIX& matView, DirectX::XMMATRIX& matProjection);

	//�v���~�e�B�u�g�|���W�[�͐��`���X�g�ɂ��邱�ƁI

};

#include "Gridline.h"
using namespace Microsoft::WRL;

void Gridline::Initialize(ComPtr<ID3D12Device> device, int lineNum, D3D12_CPU_DESCRIPTOR_HANDLE& srvHandle)
{
	HRESULT result;

	//�������烉�C���̒������擾
	Vector3 length = end - start;
	Vector3 center = (start + end) / 2.0f;
	Vector3 lineSpace = length / (float)lineNum;

	//�������璸�_�f�[�^�������o�ϐ��ɏ�������
	for (int i = 0; i < lineNum; i++) {
		Vertex ver{};
		ver.pos = { start.x + (lineSpace.x * i),center.y,start.z };
		vertices.push_back(ver);
		ver.pos = { start.x + (lineSpace.x * i) ,center.y,start.z + length.z };
		vertices.push_back(ver);
		ver.pos = { start.x ,center.y,start.z + (lineSpace.z * i) };
		vertices.push_back(ver);
		ver.pos = { start.x + length.x,center.y,start.z + (lineSpace.z * i) };
		vertices.push_back(ver);
	}

	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * vertices.size());

	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // ���_�f�[�^�S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//���_�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp{}; // �q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p

	// ���_�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&heapProp, // �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	// GPU��̃o�b�t�@�ɑΉ��������z������(���C����������)���擾
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// �S���_�ɑ΂���
	for (int i = 0; i < vertices.size(); i++) {
		vertMap[i] = vertices[i]; // ���W���R�s�[
	}
	// �q���������
	vertBuff->Unmap(0, nullptr);

	//GPU���z�A�h���X
	vbview.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//���_�o�b�t�@�̃T�C�Y
	vbview.SizeInBytes = sizeVB;
	//���_����̃f�[�^�̃T�C�Y
	vbview.StrideInBytes = sizeof(vertices[0]);

	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{  };
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;	//256�o�C�g�A���C�������g
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
		IID_PPV_ARGS(&constBuffTransform));
	assert(SUCCEEDED(result));
	//�萔�o�b�t�@�̃}�b�s���O
	result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);//�}�b�s���O
	assert(SUCCEEDED(result));
}

void Gridline::Draw(ID3D12GraphicsCommandList* commandList, ID3D12DescriptorHeap* srvheaps)
{
	//�v���~�e�B�u�g�|���W�[����`���X�g�ɃZ�b�g
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	commandList->IASetVertexBuffers(0, 1, &vbview);

	//0�Ԓ萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());

	//�`��R�}���h
	commandList->DrawInstanced(vertices.size(), 1, 0, 0);

}

void Gridline::Update(DirectX::XMMATRIX& matView, DirectX::XMMATRIX& matProjection)
{
	constMapTransform->mat = matView * matProjection;
}

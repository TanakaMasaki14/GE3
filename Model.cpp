#include"Model.h"
#include<fstream>
#include<sstream>
using namespace std;

using namespace Microsoft::WRL;
#include<cassert>
#include<DirectXMath.h>
using namespace DirectX;
#include"Texture.h"

ID3D12Device* Model::device = nullptr;

Model* Model::CreateModel(const std::string& filename)
{
	//�V�����C���X�^���X��new����
	Model* newModel = new Model();
	//obj�t�@�C������f�[�^��ǂݍ���
	newModel->Create(filename);
	//�o�b�t�@����
	newModel->CreateBuffers();

	return newModel;
}


void Model::Create(const std::string& modelname)
{
	HRESULT result;

	if (modelname != "NULL") {
		HRESULT result = S_FALSE;

		//�t�@�C���X�g���[��
		ifstream file;
		//.obj�t�@�C�����J��
		//file.open("Resources/triangle/triangle.obj");
	//	const string modelname = "triangle_mat";
		const string filename = modelname + ".obj";
		const string directoryPath = "Resources/" + modelname + "/";
		file.open(directoryPath + filename);

		//�t�@�C���I�[�v�����s���`�F�b�N
		if (file.fail()) {
			assert(0);
		}

		vector<Vector3> positions;	//���_���W
		vector<Vector3> normals;	//�@���x�N�g��
		vector<Vector2> texcords;	//�e�N�X�`��UV
		//1�s���ǂݍ���
		string line;
		while (getline(file, line)) {

			//1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
			istringstream line_stream(line);

			//���p�X�y�[�X��؂�ōs�̐擪��������擾
			string key;
			getline(line_stream, key, ' ');

			//�擪������mtllib�Ȃ�}�e���A��
			if (key == "mtllib") {
				//�}�e���A���̃t�@�C�����ǂݍ���
				string filename;
				line_stream >> filename;
				//�}�e���A���ǂݍ���
				LoadMaterial(directoryPath, filename);
			}
			//�擪������v�Ȃ璸�_���W
			if (key == "v") {
				//X,Y,Z���W�ǂݍ���
				Vector3 position{};
				line_stream >> position.x;
				line_stream >> position.y;
				line_stream >> position.z;
				//���W�f�[�^�ɒǉ�
				positions.emplace_back(position);
				//���_�f�[�^�ɒǉ�
			/*	Vertex vertex{};
				vertex.pos = position;
				vertices.emplace_back(vertex);*/
			}
			//�擪������vt�Ȃ�e�N�X�`��
			if (key == "vt") {
				//U,V�����ǂݍ���
				Vector2 texcoord{};
				line_stream >> texcoord.x;
				line_stream >> texcoord.y;
				//V�������]
				texcoord.y = 1.0f - texcoord.y;
				//�e�N�X�`�����W�f�[�^�ɒǉ�
				texcords.emplace_back(texcoord);
			}
			//�擪������vn�Ȃ�@���x�N�g��
			if (key == "vn") {
				//X,Y,Z�����ǂݍ���
				Vector3 normal{};
				line_stream >> normal.x;
				line_stream >> normal.y;
				line_stream >> normal.z;
				//�@���x�N�g���f�[�^�ɒǉ�
				normals.emplace_back(normal);
			}
			//�擪������f�Ȃ�|���S��(�O�p�`)
			if (key == "f") {
				//���p�X�y�[�X��؂�ōs�̑�����ǂݍ���
				string index_string;
				while (getline(line_stream, index_string, ' ')) {
					//���_�C���f�b�N�X1���̕�������X�g���[���ɕϊ����ĉ�͂��₷���Ȃ�
					istringstream index_stream(index_string);
					unsigned short indexPosition, indexNormal, indexTexcoord;
					index_stream >> indexPosition;
					index_stream.seekg(1, ios_base::cur);//�X���b�V�����΂�
					index_stream >> indexTexcoord;
					index_stream.seekg(1, ios_base::cur);//�X���b�V�����΂�
					index_stream >> indexNormal;
					//���_�f�[�^�̒ǉ�
					Vertex vertex{};
					vertex.pos = positions[indexPosition - 1];
					vertex.normal = normals[indexNormal - 1];
					vertex.uv = texcords[indexTexcoord - 1];
					vertices.emplace_back(vertex);
					//���_�C���f�b�N�X�ɒǉ�
					indices.emplace_back((unsigned short)indices.size());
				}

			}

		}
		//�t�@�C�������
		file.close();



	}
	else {
		//���_�f�[�^�ƒ��_�o�b�t�@�r���[�̐���
		// ���_�f�[�^
		Vertex vertices_[] = {
			//     x     y    z     �@��  u    v

				//�O
				{{-5.0f,-5.0f,-5.0f}, {},{0.0f,1.0f}}, // ����
				{{-5.0f, 5.0f,-5.0f}, {},{0.0f,0.0f}}, // ����
				{{ 5.0f,-5.0f,-5.0f}, {},{1.0f,1.0f}}, // �E��
				{{ 5.0f, 5.0f,-5.0f}, {},{1.0f,0.0f}}, // �E��

				//��
				{{-5.0f,-5.0f, 5.0f}, {},{0.0f,1.0f}}, // ����
				{{-5.0f, 5.0f, 5.0f}, {},{0.0f,0.0f}}, // ����
				{{ 5.0f,-5.0f, 5.0f}, {},{1.0f,1.0f}}, // �E��
				{{ 5.0f, 5.0f, 5.0f}, {},{1.0f,0.0f}}, // �E��

				//��
				{{-5.0f,-5.0f,-5.0f}, {} ,{0.0f,1.0f}}, // ����
				{{-5.0f,-5.0f, 5.0f}, {} ,{0.0f,0.0f}}, // ����
				{{-5.0f, 5.0f,-5.0f}, {} ,{1.0f,1.0f}}, // �E��
				{{-5.0f, 5.0f, 5.0f}, {} ,{1.0f,0.0f}}, // �E��

				//�E
				{{ 5.0f,-5.0f,-5.0f}, {} ,{0.0f,1.0f}}, // ����
				{{ 5.0f,-5.0f, 5.0f}, {} ,{0.0f,0.0f}}, // ����
				{{ 5.0f, 5.0f,-5.0f}, {} ,{1.0f,1.0f}}, // �E��
				{{ 5.0f, 5.0f, 5.0f}, {} ,{1.0f,0.0f}}, // �E��

				//��
				{{-5.0f, 5.0f,-5.0f}, {},{0.0f,1.0f}}, // ����
				{{ 5.0f, 5.0f,-5.0f}, {},{0.0f,0.0f}}, // ����
				{{-5.0f, 5.0f, 5.0f}, {},{1.0f,1.0f}}, // �E��
				{{ 5.0f, 5.0f, 5.0f}, {},{1.0f,0.0f}}, // �E��

				//��
				{{-5.0f,-5.0f,-5.0f}, {},{0.0f,1.0f}}, // ����
				{{ 5.0f,-5.0f,-5.0f}, {},{0.0f,0.0f}}, // ����
				{{-5.0f,-5.0f, 5.0f}, {},{1.0f,1.0f}}, // �E��
				{{ 5.0f,-5.0f, 5.0f}, {},{1.0f,0.0f}}, // �E��
		};
		for (int i = 0; i < _countof(vertices_); i++) {
			vertices.push_back(vertices_[i]);
		}



		//�C���f�b�N�X�f�[�^
		unsigned short indices_[] = {

			//�O
			0,1,2,		//�O�p�`1��
			2,1,3,		//�O�p�`2��
			//��	
			5,4,7,		//�O�p�`3��
			7,4,6,		//�O�p�`4��
			//��
			8,9,10,		//�O�p�`5��
			10,9,11,	//�O�p�`6��
			//�E
			13,12,15,	//�O�p�`7��
			15,12,14,	//�O�p�`8��
			//��
			17,16,19,	//�O�p�`9��
			19,16,18,	//�O�p�`10��
			//��
			20,21,22,	//�O�p�`11��
			22,21,23,	//�O�p�`12��
		};

		for (int i = 0; i < _countof(indices_); i++) {
			indices.push_back(indices_[i]);
		}

		//�@���v�Z��vec3�ɑΉ����Ă��Ȃ����߁A��������R�����g�A�E�g


		////�@���̌v�Z
		//for (int i = 0; i < indices.size() / 3; i++) {
		//	unsigned short indices0 = indices[i * 3 + 0];
		//	unsigned short indices1 = indices[i * 3 + 1];
		//	unsigned short indices2 = indices[i * 3 + 2];
		//	//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
		//	XMVECTOR p0 = XMLoadFloat3(&vertices[indices0].pos);
		//	XMVECTOR p1 = XMLoadFloat3(&vertices[indices1].pos);
		//	XMVECTOR p2 = XMLoadFloat3(&vertices[indices2].pos);
		//	//p0��p1�x�N�g���Ap0��p2�x�N�g�����v�Z�i�x�N�g���̌��Z�j
		//	XMVECTOR v1 = XMVectorSubtract(p1, p0);
		//	XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//	//�O�ς͗������琂���ȃx�N�g��
		//	XMVECTOR normal = XMVector3Cross(v1, v2);
		//	//���K��
		//	normal = XMVector3Normalize(normal);
		//	//���߂��@���𒸓_�f�[�^�ɑ��
		//	XMStoreFloat3(&vertices[indices0].normal, normal);
		//	XMStoreFloat3(&vertices[indices1].normal, normal);
		//	XMStoreFloat3(&vertices[indices2].normal, normal);
		//}

	}
	//���_�f�[�^�S�̂̃T�C�Y
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * vertices.size());

	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // ���_�f�[�^�S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// ���_�o�b�t�@�̐ݒ�
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

	// ���_�o�b�t�@�r���[�̍쐬
	// GPU���z�A�h���X
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// ���_�o�b�t�@�̃T�C�Y
	vbView.SizeInBytes = sizeVB;
	// ���_1���̃f�[�^�T�C�Y
	vbView.StrideInBytes = sizeof(vertices[0]);

	//�C���f�b�N�X�f�[�^�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(indices[0]) * indices.size());

	//���\�[�X�ݒ�
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;//�C���f�b�N�X��񂪓��镪�̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�C���f�b�N�X�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&heapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	//�C���f�b�N�X�o�b�t�@���}�b�s���O
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//�S�C���f�b�N�X�ɑ΂���
	for (int i = 0; i < indices.size(); i++) {
		indexMap[i] = indices[i];//�C���f�b�N�X���R�s�[
	}
	//�}�b�s���O����
	indexBuff->Unmap(0, nullptr);

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;


}

void Model::LoadTexture(const std::string& directoryPath, const std::string& filename)
{	//�t�@�C���p�X������
	string filePath = directoryPath + filename;

	//���j�R�[�h������ɕϊ�����
	wchar_t wfilepath[128];
	int iBufferSize = MultiByteToWideChar(CP_ACP, 0, filePath.c_str(), -1, wfilepath, _countof(wfilepath));

	textureIndex = Texture::LoadTexture(wfilepath);
}

void Model::LoadMaterial(const std::string& directoryPath, const std::string& filename)
{
	//�t�@�C���X�g���[��
	ifstream file;
	//�}�e���A���t�@�C�����J��
	file.open(directoryPath + filename);
	//�t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) {
		assert(0);
	}

	//1�s���ǂݍ���
	string line;
	while (getline(file, line)) {

		//1�s���̕�������X�g���[���ɕϊ�
		istringstream line_stream(line);

		//���p�X�y�[�X��؂�ōs�̐擪��������擾
		string key;
		getline(line_stream, key, ' ');

		//�擪�̃^�u�����͖�������
		if (key[0] == '\t') {
			key.erase(key.begin());
		}

		//�擪������newmtl�Ȃ�}�e���A����
		if (key == "newmtl") {
			//�}�e���A�����ǂݍ���
			line_stream >> material.name;
		}
		//�擪������Ka�Ȃ�A���r�G���g�F
		if (key == "Ka") {
			line_stream >> material.ambient.x;
			line_stream >> material.ambient.y;
			line_stream >> material.ambient.z;
		}
		//�擪������Kd�Ȃ�f�B�t���[�Y�F
		if (key == "Kd") {
			line_stream >> material.diffuse.x;
			line_stream >> material.diffuse.y;
			line_stream >> material.diffuse.z;
		}
		//�擪������Ks�Ȃ�X�y�L�����[�F
		if (key == "Ks") {
			line_stream >> material.specular.x;
			line_stream >> material.specular.y;
			line_stream >> material.specular.z;
		}
		//�擪������map_Kd�Ȃ�e�N�X�`���t�@�C����
		if (key == "map_Kd") {
			//�e�N�X�`���̃t�@�C�����ǂݍ���
			line_stream >> material.textureFileName;
			//�e�N�X�`���ǂݍ���
			LoadTexture(directoryPath, material.textureFileName);
		}
	}
	//�t�@�C�������
	file.close();
}

void Model::CreateBuffers()
{
	HRESULT result;
	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{  };
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;	//256�o�C�g�A���C�������g
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
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));

	ConstBufferDataMaterial* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->ambient = material.ambient;
	constMap->diffuse = material.diffuse;
	constMap->specular = material.specular;
	constMap->alpha = material.alpha;
	constBuff->Unmap(0, nullptr);

}

void Model::Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex)
{
	//���_�o�b�t�@�r���[�A�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);

	//�萔�o�b�t�@�r���[�ݒ�
	cmdList->SetGraphicsRootConstantBufferView(rootParameterIndex, constBuff->GetGPUVirtualAddress());

	//�f�X�N���v�^�q�[�v�̔z����Z�b�g����R�}���h
	ID3D12DescriptorHeap* ppHeaps[] = { Texture::descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = Texture::descHeap->GetGPUDescriptorHandleForHeapStart();
	UINT incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvGpuHandle.ptr += incrementSize * textureIndex;
	cmdList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	// �`��R�}���h
	cmdList->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);

}

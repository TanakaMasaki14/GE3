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
	//新しいインスタンスをnewする
	Model* newModel = new Model();
	//objファイルからデータを読み込む
	newModel->Create(filename);
	//バッファ生成
	newModel->CreateBuffers();

	return newModel;
}


void Model::Create(const std::string& modelname)
{
	HRESULT result;

	if (modelname != "NULL") {
		HRESULT result = S_FALSE;

		//ファイルストリーム
		ifstream file;
		//.objファイルを開く
		//file.open("Resources/triangle/triangle.obj");
	//	const string modelname = "triangle_mat";
		const string filename = modelname + ".obj";
		const string directoryPath = "Resources/" + modelname + "/";
		file.open(directoryPath + filename);

		//ファイルオープン失敗をチェック
		if (file.fail()) {
			assert(0);
		}

		vector<Vector3> positions;	//頂点座標
		vector<Vector3> normals;	//法線ベクトル
		vector<Vector2> texcords;	//テクスチャUV
		//1行ずつ読み込む
		string line;
		while (getline(file, line)) {

			//1行分の文字列をストリームに変換して解析しやすくする
			istringstream line_stream(line);

			//半角スペース区切りで行の先頭文字列を取得
			string key;
			getline(line_stream, key, ' ');

			//先頭文字列がmtllibならマテリアル
			if (key == "mtllib") {
				//マテリアルのファイル名読み込み
				string filename;
				line_stream >> filename;
				//マテリアル読み込み
				LoadMaterial(directoryPath, filename);
			}
			//先頭文字列がvなら頂点座標
			if (key == "v") {
				//X,Y,Z座標読み込み
				Vector3 position{};
				line_stream >> position.x;
				line_stream >> position.y;
				line_stream >> position.z;
				//座標データに追加
				positions.emplace_back(position);
				//頂点データに追加
			/*	Vertex vertex{};
				vertex.pos = position;
				vertices.emplace_back(vertex);*/
			}
			//先頭文字列がvtならテクスチャ
			if (key == "vt") {
				//U,V成分読み込み
				Vector2 texcoord{};
				line_stream >> texcoord.x;
				line_stream >> texcoord.y;
				//V方向反転
				texcoord.y = 1.0f - texcoord.y;
				//テクスチャ座標データに追加
				texcords.emplace_back(texcoord);
			}
			//先頭文字列がvnなら法線ベクトル
			if (key == "vn") {
				//X,Y,Z成分読み込み
				Vector3 normal{};
				line_stream >> normal.x;
				line_stream >> normal.y;
				line_stream >> normal.z;
				//法線ベクトルデータに追加
				normals.emplace_back(normal);
			}
			//先頭文字列がfならポリゴン(三角形)
			if (key == "f") {
				//半角スペース区切りで行の続きを読み込む
				string index_string;
				while (getline(line_stream, index_string, ' ')) {
					//頂点インデックス1個分の文字列をストリームに変換して解析しやすくなる
					istringstream index_stream(index_string);
					unsigned short indexPosition, indexNormal, indexTexcoord;
					index_stream >> indexPosition;
					index_stream.seekg(1, ios_base::cur);//スラッシュを飛ばす
					index_stream >> indexTexcoord;
					index_stream.seekg(1, ios_base::cur);//スラッシュを飛ばす
					index_stream >> indexNormal;
					//頂点データの追加
					Vertex vertex{};
					vertex.pos = positions[indexPosition - 1];
					vertex.normal = normals[indexNormal - 1];
					vertex.uv = texcords[indexTexcoord - 1];
					vertices.emplace_back(vertex);
					//頂点インデックスに追加
					indices.emplace_back((unsigned short)indices.size());
				}

			}

		}
		//ファイルを閉じる
		file.close();



	}
	else {
		//頂点データと頂点バッファビューの生成
		// 頂点データ
		Vertex vertices_[] = {
			//     x     y    z     法線  u    v

				//前
				{{-5.0f,-5.0f,-5.0f}, {},{0.0f,1.0f}}, // 左下
				{{-5.0f, 5.0f,-5.0f}, {},{0.0f,0.0f}}, // 左上
				{{ 5.0f,-5.0f,-5.0f}, {},{1.0f,1.0f}}, // 右下
				{{ 5.0f, 5.0f,-5.0f}, {},{1.0f,0.0f}}, // 右上

				//後
				{{-5.0f,-5.0f, 5.0f}, {},{0.0f,1.0f}}, // 左下
				{{-5.0f, 5.0f, 5.0f}, {},{0.0f,0.0f}}, // 左上
				{{ 5.0f,-5.0f, 5.0f}, {},{1.0f,1.0f}}, // 右下
				{{ 5.0f, 5.0f, 5.0f}, {},{1.0f,0.0f}}, // 右上

				//左
				{{-5.0f,-5.0f,-5.0f}, {} ,{0.0f,1.0f}}, // 左下
				{{-5.0f,-5.0f, 5.0f}, {} ,{0.0f,0.0f}}, // 左上
				{{-5.0f, 5.0f,-5.0f}, {} ,{1.0f,1.0f}}, // 右下
				{{-5.0f, 5.0f, 5.0f}, {} ,{1.0f,0.0f}}, // 右上

				//右
				{{ 5.0f,-5.0f,-5.0f}, {} ,{0.0f,1.0f}}, // 左下
				{{ 5.0f,-5.0f, 5.0f}, {} ,{0.0f,0.0f}}, // 左上
				{{ 5.0f, 5.0f,-5.0f}, {} ,{1.0f,1.0f}}, // 右下
				{{ 5.0f, 5.0f, 5.0f}, {} ,{1.0f,0.0f}}, // 右上

				//下
				{{-5.0f, 5.0f,-5.0f}, {},{0.0f,1.0f}}, // 左下
				{{ 5.0f, 5.0f,-5.0f}, {},{0.0f,0.0f}}, // 左上
				{{-5.0f, 5.0f, 5.0f}, {},{1.0f,1.0f}}, // 右下
				{{ 5.0f, 5.0f, 5.0f}, {},{1.0f,0.0f}}, // 右上

				//上
				{{-5.0f,-5.0f,-5.0f}, {},{0.0f,1.0f}}, // 左下
				{{ 5.0f,-5.0f,-5.0f}, {},{0.0f,0.0f}}, // 左上
				{{-5.0f,-5.0f, 5.0f}, {},{1.0f,1.0f}}, // 右下
				{{ 5.0f,-5.0f, 5.0f}, {},{1.0f,0.0f}}, // 右上
		};
		for (int i = 0; i < _countof(vertices_); i++) {
			vertices.push_back(vertices_[i]);
		}



		//インデックスデータ
		unsigned short indices_[] = {

			//前
			0,1,2,		//三角形1つ目
			2,1,3,		//三角形2つ目
			//後	
			5,4,7,		//三角形3つ目
			7,4,6,		//三角形4つ目
			//左
			8,9,10,		//三角形5つ目
			10,9,11,	//三角形6つ目
			//右
			13,12,15,	//三角形7つ目
			15,12,14,	//三角形8つ目
			//下
			17,16,19,	//三角形9つ目
			19,16,18,	//三角形10つ目
			//上
			20,21,22,	//三角形11つ目
			22,21,23,	//三角形12つ目
		};

		for (int i = 0; i < _countof(indices_); i++) {
			indices.push_back(indices_[i]);
		}

		//法線計算がvec3に対応していないため、いったんコメントアウト


		////法線の計算
		//for (int i = 0; i < indices.size() / 3; i++) {
		//	unsigned short indices0 = indices[i * 3 + 0];
		//	unsigned short indices1 = indices[i * 3 + 1];
		//	unsigned short indices2 = indices[i * 3 + 2];
		//	//三角形を構成する頂点座標をベクトルに代入
		//	XMVECTOR p0 = XMLoadFloat3(&vertices[indices0].pos);
		//	XMVECTOR p1 = XMLoadFloat3(&vertices[indices1].pos);
		//	XMVECTOR p2 = XMLoadFloat3(&vertices[indices2].pos);
		//	//p0→p1ベクトル、p0→p2ベクトルを計算（ベクトルの減算）
		//	XMVECTOR v1 = XMVectorSubtract(p1, p0);
		//	XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//	//外積は両方から垂直なベクトル
		//	XMVECTOR normal = XMVector3Cross(v1, v2);
		//	//正規化
		//	normal = XMVector3Normalize(normal);
		//	//求めた法線を頂点データに代入
		//	XMStoreFloat3(&vertices[indices0].normal, normal);
		//	XMStoreFloat3(&vertices[indices1].normal, normal);
		//	XMStoreFloat3(&vertices[indices2].normal, normal);
		//}

	}
	//頂点データ全体のサイズ
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * vertices.size());

	// リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // 頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{}; // ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用

	// 頂点バッファの生成
	result = device->CreateCommittedResource(
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// 全頂点に対して
	for (int i = 0; i < vertices.size(); i++) {
		vertMap[i] = vertices[i]; // 座標をコピー
	}
	// 繋がりを解除
	vertBuff->Unmap(0, nullptr);

	// 頂点バッファビューの作成
	// GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	// 頂点1つ分のデータサイズ
	vbView.StrideInBytes = sizeof(vertices[0]);

	//インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(indices[0]) * indices.size());

	//リソース設定
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;//インデックス情報が入る分のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//インデックスバッファの生成
	result = device->CreateCommittedResource(
		&heapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	//インデックスバッファをマッピング
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	//全インデックスに対して
	for (int i = 0; i < indices.size(); i++) {
		indexMap[i] = indices[i];//インデックスをコピー
	}
	//マッピング解除
	indexBuff->Unmap(0, nullptr);

	//インデックスバッファビューの作成
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;


}

void Model::LoadTexture(const std::string& directoryPath, const std::string& filename)
{	//ファイルパスを結合
	string filePath = directoryPath + filename;

	//ユニコード文字列に変換する
	wchar_t wfilepath[128];
	int iBufferSize = MultiByteToWideChar(CP_ACP, 0, filePath.c_str(), -1, wfilepath, _countof(wfilepath));

	textureIndex = Texture::LoadTexture(wfilepath);
}

void Model::LoadMaterial(const std::string& directoryPath, const std::string& filename)
{
	//ファイルストリーム
	ifstream file;
	//マテリアルファイルを開く
	file.open(directoryPath + filename);
	//ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	//1行ずつ読み込む
	string line;
	while (getline(file, line)) {

		//1行分の文字列をストリームに変換
		istringstream line_stream(line);

		//半角スペース区切りで行の先頭文字列を取得
		string key;
		getline(line_stream, key, ' ');

		//先頭のタブ文字は無視する
		if (key[0] == '\t') {
			key.erase(key.begin());
		}

		//先頭文字列がnewmtlならマテリアル名
		if (key == "newmtl") {
			//マテリアル名読み込み
			line_stream >> material.name;
		}
		//先頭文字列がKaならアンビエント色
		if (key == "Ka") {
			line_stream >> material.ambient.x;
			line_stream >> material.ambient.y;
			line_stream >> material.ambient.z;
		}
		//先頭文字列がKdならディフューズ色
		if (key == "Kd") {
			line_stream >> material.diffuse.x;
			line_stream >> material.diffuse.y;
			line_stream >> material.diffuse.z;
		}
		//先頭文字列がKsならスペキュラー色
		if (key == "Ks") {
			line_stream >> material.specular.x;
			line_stream >> material.specular.y;
			line_stream >> material.specular.z;
		}
		//先頭文字列がmap_Kdならテクスチャファイル名
		if (key == "map_Kd") {
			//テクスチャのファイル名読み込み
			line_stream >> material.textureFileName;
			//テクスチャ読み込み
			LoadTexture(directoryPath, material.textureFileName);
		}
	}
	//ファイルを閉じる
	file.close();
}

void Model::CreateBuffers()
{
	HRESULT result;
	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{  };
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;	//256バイトアラインメント
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//定数バッファの生成
	result = device->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//リソース設定
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
	//頂点バッファビュー、インデックスバッファビューの設定
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	cmdList->IASetIndexBuffer(&ibView);

	//定数バッファビュー設定
	cmdList->SetGraphicsRootConstantBufferView(rootParameterIndex, constBuff->GetGPUVirtualAddress());

	//デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { Texture::descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = Texture::descHeap->GetGPUDescriptorHandleForHeapStart();
	UINT incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvGpuHandle.ptr += incrementSize * textureIndex;
	cmdList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	// 描画コマンド
	cmdList->DrawIndexedInstanced((UINT)indices.size(), 1, 0, 0, 0);

}

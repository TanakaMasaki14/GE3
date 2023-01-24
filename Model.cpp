#include "Model.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <d3dx12.h>

void Model::LoadFromOBJInternal(const char* folderName, const bool smoothing)
{
	//ファイルストリーム
	std::ifstream file;
	//
	const std::string modelname = folderName;
	const std::string filename = modelname + ".obj";//"ファイル名.obj"
	const std::string directoryPath = "Resources/" + modelname + "/";// "Resources/フォルダ名/"
	//.objファイルを開く
	file.open(directoryPath + filename);// "Resources/フォルダ名/ファイル名.obj"
	//ファイルオープン失敗をチェック
	assert(file.is_open());

	std::stringstream all;
	//ファイルの内容を文字列ストリームにコピー
	all << file.rdbuf();

	//ファイルを閉じる
	file.close();


	//vertex用
	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> texcoords;

	//1行分の文字列を入れる変数
	std::string line;

	int indexCount = 0;

	while (std::getline(all, line))
	{
		//一行分の文字列をストリームに変換して解析しやすく
		std::istringstream line_stream(line);

		//半角スペース区切りで行の先頭文字列を取得
		std::string key;
		getline(line_stream, key, ' ');



		//先頭文字が"mtllib"ならマテリアル
		if (key == "mtllib")
		{
			//マテリアルのファイル名読み込み・・
			std::string filename;
			line_stream >> filename;
			//マテリアル読み込み
			LoadMaterial(directoryPath, filename);
		}
		//'v'なら頂点座標
		if (key == "v")
		{
			//x,y,z座標読み込み
			XMFLOAT3 pos{};
			line_stream >> pos.x;
			line_stream >> pos.y;
			line_stream >> pos.z;
			//座標データに追加
			positions.emplace_back(pos);
		}
		//テクスチャ
		if (key == "vt")
		{
			//UV成分読み込み
			XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			//V方向反転
			texcoord.y = 1.0f - texcoord.y;
			//テクスチャ座標データに追加
			texcoords.emplace_back(texcoord);
		}
		//法線ベクトル
		if (key == "vn")
		{
			//X,Y,Z成分読み込み
			XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			//法線ベクトルデータに追加
			normals.emplace_back(normal);
		}
		//ポリゴン
		if (key == "f")
		{
			int count = 0;

			//半角スペース区切りで行の続きを読み込む
			std::string index_string;
			while (getline(line_stream, index_string, ' '))
			{
				//頂点インデックス一個分の文字列をストリームに変換して解析しやすく
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				index_stream >> indexPosition;
				//頂点インデックス等に追加(頂点インデックス、法線、uvの順)
				//(1// 2// 3//　←("//")には非対応)
				index_stream.seekg(1, std::ios_base::cur);//スラッシュ(/)を飛ばす
				index_stream >> indexTexcoord;
				index_stream.seekg(1, std::ios_base::cur);//スラッシュを飛ばす
				index_stream >> indexNormal;

				//頂点データの追加
				Vertex vertex{};
				vertex.pos = positions[indexPosition - 1];
				vertex.normal = normals[indexNormal - 1];
				vertex.uv = texcoords[indexTexcoord - 1];
				verticesM.emplace_back(vertex);

				//エッジ平滑化用のデータを追加
				if (smoothing)
				{
					//vキー（座標データ）の番号と、全て合成した頂点のインデックスをセットで登録する
					AddSmoothData(indexPosition, (unsigned short)GetVertexCount() - 1);
				}

				//インデックスデータの追加
				count++;

				indicesM.emplace_back((unsigned short)indicesM.size() - indexCount);
				if (count >= 4)
				{
					indicesM.emplace_back((unsigned short)indicesM.size() - 4 - indexCount);
					indicesM.emplace_back((unsigned short)indicesM.size() - 3 - indexCount);
					indexCount += 2;
					count = 0;
					break;
				}

			}
		}
	}
	//頂点法線の平均によるエッジの平滑化
	if (smoothing)
	{
		CalculateSmoothedVertexNormals();
	}
}

void Model::CreateBuffers()
{
	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};

	//AL4_02_02
	{
		ResourceProperties(cbResourceDesc,
			((UINT)sizeof(ConstBufferDataMaterial2) + 0xff) & ~0xff/*256バイトアライメント*/);
		//定数バッファの生成
		BuffProperties(cbHeapProp, cbResourceDesc, &constBuffMaterial2);
		//定数バッファのマッピング
		Directx::GetInstance().result = constBuffMaterial2->Map(0, nullptr, (void**)&constMapMaterial2);//マッピング
		assert(SUCCEEDED(Directx::GetInstance().result));
		//マテリアルをセット(毎フレーム変わる様なものじゃないので、updateに入れない)
		constMapMaterial2->ambient = material.ambient;
		constMapMaterial2->diffuse = material.diffuse;
		constMapMaterial2->specular = material.specular;
		constMapMaterial2->alpha = material.alpha;

		constBuffMaterial2->Unmap(0, nullptr);
	}


	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * verticesM.size());
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indicesM.size());

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

	// 頂点バッファ生成
	Directx::GetInstance().result = Directx::GetInstance().GetDevice()->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuffM));
	assert(SUCCEEDED(Directx::GetInstance().result));


	// 頂点バッファへのデータ転送
	Vertex* vertMap = nullptr;
	Directx::GetInstance().result = vertBuffM->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(Directx::GetInstance().result)) {
		std::copy(verticesM.begin(), verticesM.end(), vertMap);
		vertBuffM->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbViewM.BufferLocation = vertBuffM->GetGPUVirtualAddress();
	vbViewM.SizeInBytes = sizeVB;
	vbViewM.StrideInBytes = sizeof(Vertex);

	// リソース設定
	resourceDesc.Width = sizeIB;

	// インデックスバッファ生成
	BuffProperties(heapProps, resourceDesc, &indexBuffM);

	// インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	Directx::GetInstance().result = indexBuffM->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(Directx::GetInstance().result)) {

		std::copy(indicesM.begin(), indicesM.end(), indexMap);

		indexBuffM->Unmap(0, nullptr);
	}

	// インデックスバッファビューの作成
	ibViewM.BufferLocation = indexBuffM->GetGPUVirtualAddress();
	ibViewM.Format = DXGI_FORMAT_R16_UINT;
	ibViewM.SizeInBytes = sizeIB;
}

void Model::LoadMaterial(const std::string& directoryPath, const std::string& filename)
{
	//ファイルストリーム
	std::ifstream file;
	//マテリアルファイルを開く
	file.open(directoryPath + filename);
	//ファイルオープン失敗をチェック
	if (file.fail()) assert(0);

	//一行ずつ読み込む
	std::string line;
	while (std::getline(file, line))
	{
		//一行分の文字列をストリームに変換
		std::istringstream line_stream(line);

		//半角スペース区切りで行の先頭文字列を取得
		std::string key;
		std::getline(line_stream, key, ' ');

		//先頭のtab文字は無視
		if (key[0] == '\t')
		{
			key.erase(key.begin());
		}

		//先頭文字列が"newmtl"ならマテリアル名
		if (key == "newmtl")
		{
			//マテリアル名読み込み
			line_stream >> material.name;
		}
		//"Ka"ならアンビエント色
		if (key == "Ka")
		{
			line_stream >> material.ambient.x;
			line_stream >> material.ambient.y;
			line_stream >> material.ambient.z;
		}
		//"Kd"ならディフューズ色
		if (key == "Kd")
		{
			line_stream >> material.diffuse.x;
			line_stream >> material.diffuse.y;
			line_stream >> material.diffuse.z;
		}
		//"Ks"ならスペキュラー色
		if (key == "Ks")
		{
			line_stream >> material.specular.x;
			line_stream >> material.specular.y;
			line_stream >> material.specular.z;
		}
		//"map_Kd"ならテクスチャファイル名
		if (key == "map_Kd")
		{
			//テクスチャファイル名読み込み
			line_stream >> material.textureFilename;
			//テクスチャ読み込み
			std::string nameS = directoryPath + material.textureFilename;
			//
			const char* name = nameS.c_str();
			wchar_t wchar[128];
			size_t size = _countof(wchar);
			mbstowcs_s(&size, wchar, name, size);
			TextureManager::GetInstance().LoadGraph(wchar, material.textureHandle);
		}
	}
	file.close();
}


Model* Model::LoadFromOBJ(const char* folderName, const bool smoothing)
{
	//新たなModel型のインスタンスのメモリを確保
	Model* model = new Model();
	//読み込み
	model->LoadFromOBJInternal(folderName, smoothing);
	//バッファ生成
	model->CreateBuffers();

	return model;
}

void Model::AddSmoothData(unsigned short indexPosition, unsigned short indexVertex)
{
	smoothData_[indexPosition].emplace_back(indexVertex);
}

void Model::CalculateSmoothedVertexNormals()
{
	auto itr = smoothData_.begin();
	for (; itr != smoothData_.end(); itr++)
	{
		//各面用の共通頂点コレクション
		std::vector<unsigned short>& v = itr->second;
		//全頂点の法線を平均する
		XMVECTOR normal = {};
		for (unsigned short index : v)
		{
			normal += XMVectorSet(verticesM[index].normal.x, verticesM[index].normal.y, verticesM[index].normal.z, 0);
		}
		normal = XMVector3Normalize(normal / (float)v.size());
		//共通法線を使用するすべてのデータに書き込む
		for (unsigned short index : v)
		{
			verticesM[index].normal = { normal.m128_f32[0],normal.m128_f32[1] ,normal.m128_f32[2] };
		}
	}
}

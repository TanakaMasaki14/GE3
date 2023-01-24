#include "Primitive.h"
#include "Util.h"



UINT sizeVB;

void Primitive::Initialize()
{
	InitializeTriangle();
	InitializeBox();
	InitializeCircle();
	InitializeLine();
	InitializeCube();
	InitializeSphere();
}

void Primitive::InitializeTriangle()
{
	verticesTriangle[0] = { {-5,-5,0},{verticesTriangle[0].normal},{0.0f,1.0f} };//左下
	verticesTriangle[1] = { {0,5,0},{verticesTriangle[1].normal},{0.5f,0.0f} };//上
	verticesTriangle[2] = { {5,-5,0},{verticesTriangle[2].normal},{1.0f,1.0f} };//右下


	// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
	sizeVB = static_cast<UINT>(sizeof(verticesTriangle[0]) * _countof(verticesTriangle));

	//頂点バッファの設定		//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

	ResourceProperties(TextureManager::GetInstance().resDesc, sizeVB);

	//頂点バッファの生成
	BuffProperties(heapProp, TextureManager::GetInstance().resDesc, &vertBuffTriangle);

	// 頂点バッファビューの作成
	// GPU仮想アドレス
	vbTriangleView.BufferLocation = vertBuffTriangle->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbTriangleView.SizeInBytes = sizeVB;
	// 頂点1つ分のデータサイズ
	vbTriangleView.StrideInBytes = sizeof(verticesTriangle[0]);

	{
		//03_04
		//インデックスデータ
		//インデックスデータ全体のサイズ
		UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indicesTriangle));

		//リソース設定
		ResourceProperties(TextureManager::GetInstance().resDesc, sizeIB);
		//インデックスバッファの作成
		ID3D12Resource* indexBuff = nullptr;//GPU側のメモリ
		BuffProperties(heapProp, TextureManager::GetInstance().resDesc, &indexBuff);
		//インデックスバッファをマッピング
		uint16_t* indexMap = nullptr;
		Directx::GetInstance().result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//全インデックスに対して
		for (int i = 0; i < _countof(indicesTriangle); i++)
		{
			indexMap[i] = indicesTriangle[i];//インデックスをコピー

		}
		//マッピングを解除
		indexBuff->Unmap(0, nullptr);

		//インデックスバッファビューの作成
		ibViewTriangle.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibViewTriangle.Format = DXGI_FORMAT_R16_UINT;
		ibViewTriangle.SizeInBytes = sizeIB;
	}

	//06_03
	{
		for (int i = 0; i < _countof(indicesTriangle) / 3; i++)
		{//三角形一つごとに計算
			//三角形のインデックスを取り出して、一時的な変数に入れる
			unsigned short index0 = indicesTriangle[i * 3 + 0];
			unsigned short index1 = indicesTriangle[i * 3 + 1];
			unsigned short index2 = indicesTriangle[i * 3 + 2];
			//三角形を構成する頂点座標をベクトルに代入
			XMVECTOR p0 = XMLoadFloat3(&verticesTriangle[index0].pos);
			XMVECTOR p1 = XMLoadFloat3(&verticesTriangle[index1].pos);
			XMVECTOR p2 = XMLoadFloat3(&verticesTriangle[index2].pos);
			//p0->p1ベクトル、p0->p2ベクトルを計算
			XMVECTOR v1 = XMVectorSubtract(p1, p0);
			XMVECTOR v2 = XMVectorSubtract(p2, p0);
			//外積（垂直なベクトル）
			XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
			//求めた法線を頂点データに代入
			XMStoreFloat3(&verticesTriangle[index0].normal, normal);
			XMStoreFloat3(&verticesTriangle[index1].normal, normal);
			XMStoreFloat3(&verticesTriangle[index2].normal, normal);
		}
	}
}

void Primitive::InitializeBox()
{
	// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
	sizeVB = static_cast<UINT>(sizeof(verticesBox[0]) * _countof(verticesBox));

	//頂点バッファの設定		//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

	ResourceProperties(TextureManager::GetInstance().resDesc, sizeVB);

	//頂点バッファの生成
	BuffProperties(heapProp, TextureManager::GetInstance().resDesc, &vertBuffBox);

	// 頂点バッファビューの作成
	// GPU仮想アドレス
	vbBoxView.BufferLocation = vertBuffBox->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbBoxView.SizeInBytes = sizeVB;
	// 頂点1つ分のデータサイズ
	vbBoxView.StrideInBytes = sizeof(verticesBox[0]);

	{
		//03_04
		//インデックスデータ
		//インデックスデータ全体のサイズ
		UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indicesBox));

		//リソース設定
		ResourceProperties(TextureManager::GetInstance().resDesc, sizeIB);
		//インデックスバッファの作成
		ID3D12Resource* indexBuff = nullptr;//GPU側のメモリ
		BuffProperties(heapProp, TextureManager::GetInstance().resDesc, &indexBuff);
		//インデックスバッファをマッピング
		uint16_t* indexMap = nullptr;
		Directx::GetInstance().result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//全インデックスに対して
		for (int i = 0; i < _countof(indicesBox); i++)
		{
			indexMap[i] = indicesBox[i];//インデックスをコピー

		}
		//マッピングを解除
		indexBuff->Unmap(0, nullptr);

		//インデックスバッファビューの作成
		ibViewBox.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibViewBox.Format = DXGI_FORMAT_R16_UINT;
		ibViewBox.SizeInBytes = sizeIB;
	}

	//06_03
	{
		for (int i = 0; i < _countof(indicesBox) / 3; i++)
		{//三角形一つごとに計算
			//三角形のインデックスを取り出して、一時的な変数に入れる
			unsigned short index0 = indicesBox[i * 3 + 0];
			unsigned short index1 = indicesBox[i * 3 + 1];
			unsigned short index2 = indicesBox[i * 3 + 2];
			//三角形を構成する頂点座標をベクトルに代入
			XMVECTOR p0 = XMLoadFloat3(&verticesBox[index0].pos);
			XMVECTOR p1 = XMLoadFloat3(&verticesBox[index1].pos);
			XMVECTOR p2 = XMLoadFloat3(&verticesBox[index2].pos);
			//p0->p1ベクトル、p0->p2ベクトルを計算
			XMVECTOR v1 = XMVectorSubtract(p1, p0);
			XMVECTOR v2 = XMVectorSubtract(p2, p0);
			//外積（垂直なベクトル）
			XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
			//求めた法線を頂点データに代入
			XMStoreFloat3(&verticesBox[index0].normal, normal);
			XMStoreFloat3(&verticesBox[index1].normal, normal);
			XMStoreFloat3(&verticesBox[index2].normal, normal);
		}
	}
}

void Primitive::InitializeCircle()
{
	verticesCircle[0].pos = { 0.0f,0.0f,0.0f };

	static float count = _countof(verticesCircle) - 2;//中心点と初期の点はカウントしない

	for (int i = 1; i < _countof(verticesCircle); i++)
	{
		verticesCircle[i].pos = { 1.0f * cosf(AngletoRadi(360 / count) * (i - 1)),1.0f * sinf(AngletoRadi(360 / count) * (i - 1)),0 };
	}

	// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
	sizeVB = static_cast<UINT>(sizeof(verticesCircle[0]) * _countof(verticesCircle));

	//頂点バッファの設定		//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

	ResourceProperties(TextureManager::GetInstance().resDesc, sizeVB);

	//頂点バッファの生成
	BuffProperties(heapProp, TextureManager::GetInstance().resDesc, &vertBuffCircle);

	// 頂点バッファビューの作成
	// GPU仮想アドレス
	vbCircleView.BufferLocation = vertBuffCircle->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbCircleView.SizeInBytes = sizeVB;
	// 頂点1つ分のデータサイズ
	vbCircleView.StrideInBytes = sizeof(verticesCircle[0]);

	{
		//03_04
		//インデックスデータ
		//インデックスデータ全体のサイズ
		UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indicesCircle));

		//リソース設定
		ResourceProperties(TextureManager::GetInstance().resDesc, sizeIB);
		//インデックスバッファの作成
		ID3D12Resource* indexBuff = nullptr;//GPU側のメモリ
		BuffProperties(heapProp, TextureManager::GetInstance().resDesc, &indexBuff);
		//インデックスバッファをマッピング
		uint16_t* indexMap = nullptr;
		Directx::GetInstance().result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//全インデックスに対して
		for (int i = 0; i < _countof(indicesCircle); i++)
		{
			indexMap[i] = indicesCircle[i];//インデックスをコピー

		}
		//マッピングを解除
		indexBuff->Unmap(0, nullptr);

		//インデックスバッファビューの作成
		ibViewCircle.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibViewCircle.Format = DXGI_FORMAT_R16_UINT;
		ibViewCircle.SizeInBytes = sizeIB;
	}

	//06_03
	{
		for (int i = 0; i < _countof(indicesCircle) / 3; i++)
		{//三角形一つごとに計算
			//三角形のインデックスを取り出して、一時的な変数に入れる
			unsigned short index0 = indicesCircle[i * 3 + 0];
			unsigned short index1 = indicesCircle[i * 3 + 1];
			unsigned short index2 = indicesCircle[i * 3 + 2];
			//三角形を構成する頂点座標をベクトルに代入
			XMVECTOR p0 = XMLoadFloat3(&verticesCircle[index0].pos);
			XMVECTOR p1 = XMLoadFloat3(&verticesCircle[index1].pos);
			XMVECTOR p2 = XMLoadFloat3(&verticesCircle[index2].pos);
			//p0->p1ベクトル、p0->p2ベクトルを計算
			XMVECTOR v1 = XMVectorSubtract(p1, p0);
			XMVECTOR v2 = XMVectorSubtract(p2, p0);
			//外積（垂直なベクトル）
			XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
			//求めた法線を頂点データに代入
			XMStoreFloat3(&verticesCircle[index0].normal, normal);
			XMStoreFloat3(&verticesCircle[index1].normal, normal);
			XMStoreFloat3(&verticesCircle[index2].normal, normal);
		}
	}
}

void Primitive::InitializeCube()
{
	{
		//手前
		verticesCube[0] = { {-5.0f,-5.0f,-5.0f},{verticesCube[0].normal},{0.0f,1.0f} };//左下
		verticesCube[1] = { {-5.0f,5.0f, -5.0f},{verticesCube[1].normal},{0.0f,0.0f} };//左上
		verticesCube[2] = { {5.0f,-5.0f, -5.0f},{verticesCube[2].normal},{1.0f,1.0f} };//右下
		verticesCube[3] = { {5.0f,5.0f,  -5.0f},{verticesCube[3].normal},{1.0f,0.0f} };//右上

		verticesCube[4] = { {-5.0f,-5.0f,5.0f},{verticesCube[4].normal},{0.0f,1.0f} };//左下
		verticesCube[5] = { {-5.0f,5.0f, 5.0f},{verticesCube[5].normal},{0.0f,0.0f} };//左上
		verticesCube[6] = { {5.0f,-5.0f, 5.0f},{verticesCube[6].normal},{1.0f,1.0f} };//右下
		verticesCube[7] = { {5.0f,5.0f,  5.0f},{verticesCube[7].normal},{1.0f,0.0f} };//右上
			//上
		verticesCube[8] = { {5.0f,5.0f,-5.0f},{verticesCube[8].normal},{0.0f,1.0f} };//左下
		verticesCube[9] = { {5.0f,5.0f, 5.0f},{verticesCube[9].normal},{0.0f,0.0f} };//左上
		verticesCube[10] = { {-5.0f,5.0f, -5.0f},{verticesCube[10].normal},{1.0f,1.0f} };//右下
		verticesCube[11] = { {-5.0f,5.0f, 5.0f},{verticesCube[11].normal},{1.0f,0.0f} };//右上

		verticesCube[12] = { {5.0f,-5.0f,-5.0f},{verticesCube[12].normal},{0.0f,1.0f} };//左下
		verticesCube[13] = { {5.0f,-5.0f, 5.0f},{verticesCube[13].normal},{0.0f,0.0f} };//左上
		verticesCube[14] = { {-5.0f,-5.0f, -5.0f},{verticesCube[14].normal},{1.0f,1.0f} };//右下
		verticesCube[15] = { {-5.0f,-5.0f, 5.0f},{verticesCube[15].normal},{1.0f,0.0f} };//右上

		verticesCube[16] = { {-5.0f,-5.0f,-5.0f},{verticesCube[16].normal},{0.0f,1.0f} };//左下
		verticesCube[17] = { {-5.0f,-5.0f, 5.0f},{verticesCube[17].normal},{0.0f,0.0f} };//左上
		verticesCube[18] = { {-5.0f,5.0f, -5.0f},{verticesCube[18].normal},{1.0f,1.0f} };//右下
		verticesCube[19] = { {-5.0f,5.0f,  5.0f},{verticesCube[19].normal},{1.0f,0.0f} };//右上

		verticesCube[20] = { {5.0f,-5.0f,-5.0f},{verticesCube[20].normal},{0.0f,1.0f} };//左下
		verticesCube[21] = { {5.0f,-5.0f, 5.0f},{verticesCube[21].normal},{0.0f,0.0f} };//左上
		verticesCube[22] = { {5.0f,5.0f, -5.0f},{verticesCube[22].normal},{1.0f,1.0f} };//右下
		verticesCube[23] = { {5.0f,5.0f,  5.0f},{verticesCube[23].normal},{1.0f,0.0f} };//右上;//左下
	}

	// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
	sizeVB = static_cast<UINT>(sizeof(verticesCube[0]) * _countof(verticesCube));

	//頂点バッファの設定		//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

	ResourceProperties(TextureManager::GetInstance().resDesc, sizeVB);

	//頂点バッファの生成
	BuffProperties(heapProp, TextureManager::GetInstance().resDesc, &vertBuffCube);

	// 頂点バッファビューの作成
	// GPU仮想アドレス
	vbCubeView.BufferLocation = vertBuffCube->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbCubeView.SizeInBytes = sizeVB;
	// 頂点1つ分のデータサイズ
	vbCubeView.StrideInBytes = sizeof(verticesCube[0]);

	{
		//03_04
		//インデックスデータ
		//インデックスデータ全体のサイズ
		UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indicesCube));

		//リソース設定
		ResourceProperties(TextureManager::GetInstance().resDesc, sizeIB);
		//インデックスバッファの作成
		ID3D12Resource* indexBuff = nullptr;//GPU側のメモリ
		BuffProperties(heapProp, TextureManager::GetInstance().resDesc, &indexBuff);
		//インデックスバッファをマッピング
		uint16_t* indexMap = nullptr;
		Directx::GetInstance().result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//全インデックスに対して
		for (int i = 0; i < _countof(indicesCube); i++)
		{
			indexMap[i] = indicesCube[i];//インデックスをコピー

		}
		//マッピングを解除
		indexBuff->Unmap(0, nullptr);

		//インデックスバッファビューの作成
		ibViewCube.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibViewCube.Format = DXGI_FORMAT_R16_UINT;
		ibViewCube.SizeInBytes = sizeIB;
	}


	//06_03
		/*if (indexNum == SPHERE)*/
	{
		for (int i = 0; i < _countof(indicesCube) / 3; i++)
		{//三角形一つごとに計算
			//三角形のインデックスを取り出して、一時的な変数に入れる
			unsigned short index0 = indicesCube[i * 3 + 0];
			unsigned short index1 = indicesCube[i * 3 + 1];
			unsigned short index2 = indicesCube[i * 3 + 2];
			//三角形を構成する頂点座標をベクトルに代入
			XMVECTOR p0 = XMLoadFloat3(&verticesCube[index0].pos);
			XMVECTOR p1 = XMLoadFloat3(&verticesCube[index1].pos);
			XMVECTOR p2 = XMLoadFloat3(&verticesCube[index2].pos);
			//p0->p1ベクトル、p0->p2ベクトルを計算
			XMVECTOR v1 = XMVectorSubtract(p1, p0);
			XMVECTOR v2 = XMVectorSubtract(p2, p0);
			//外積（垂直なベクトル）
			XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
			//求めた法線を頂点データに代入
			XMStoreFloat3(&verticesCube[index0].normal, normal);
			XMStoreFloat3(&verticesCube[index1].normal, normal);
			XMStoreFloat3(&verticesCube[index2].normal, normal);
		}
	}
}

void Primitive::InitializeLine()
{
	verticesLine[0] = { {-5,0,0},{verticesLine[0].normal},{0.0f,1.0f} };//左下
	verticesLine[1] = { {5,0,0},  {verticesLine[1].normal},{0.5f,0.0f} };//上

	{
		//03_04
		//インデックスデータ
		//インデックスデータ全体のサイズ
		UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indicesLine));

		//リソース設定
		ResourceProperties(TextureManager::GetInstance().resDesc, sizeIB);
		//インデックスバッファの作成
		ID3D12Resource* indexBuff = nullptr;//GPU側のメモリ
		BuffProperties(heapProp, TextureManager::GetInstance().resDesc, &indexBuff);
		//インデックスバッファをマッピング
		uint16_t* indexMap = nullptr;
		Directx::GetInstance().result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//全インデックスに対して
		for (int i = 0; i < _countof(indicesLine); i++)
		{
			indexMap[i] = indicesLine[i];//インデックスをコピー

		}
		//マッピングを解除
		indexBuff->Unmap(0, nullptr);

		//インデックスバッファビューの作成
		ibViewLine.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibViewLine.Format = DXGI_FORMAT_R16_UINT;
		ibViewLine.SizeInBytes = sizeIB;
	}

	// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
	sizeVB = static_cast<UINT>(sizeof(verticesLine[0]) * _countof(verticesLine));

	//頂点バッファの設定		//ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

	ResourceProperties(TextureManager::GetInstance().resDesc, sizeVB);

	//頂点バッファの生成
	BuffProperties(heapProp, TextureManager::GetInstance().resDesc, &vertBuffLine);

	// 頂点バッファビューの作成
	// GPU仮想アドレス
	vbLineView.BufferLocation = vertBuffLine->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbLineView.SizeInBytes = sizeVB;
	// 頂点1つ分のデータサイズ
	vbLineView.StrideInBytes = sizeof(verticesLine[0]);
}

void Primitive::InitializeSphere()
{
	//球体用
	{
		// 頂点データ全体のサイズ = 頂点データ1つ分のサイズ * 頂点データの要素数
		sizeVB = static_cast<UINT>(sizeof(verticesSphere[0]) * (_countof(verticesSphere)));

		//頂点バッファの設定		//ヒープ設定
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;		//GPUへの転送用

		ResourceProperties(TextureManager::GetInstance().resDesc, sizeVB);

		//頂点バッファの生成
		BuffProperties(heapProp, TextureManager::GetInstance().resDesc, vertBuffSphere.GetAddressOf());

		// 頂点バッファビューの作成
		// GPU仮想アドレス
		vbViewSphere.BufferLocation = vertBuffSphere->GetGPUVirtualAddress();
		// 頂点バッファのサイズ
		vbViewSphere.SizeInBytes = sizeVB;
		// 頂点1つ分のデータサイズ
		vbViewSphere.StrideInBytes = sizeof(verticesSphere[0]);


		WorldMat worldMat;
		Vec3 vec = { 0,-5.0f,0 };

		//頂点二つ
		verticesSphere[0] = { {vec.x,vec.y,vec.z},{},{1.0f,0.0f} };//下
		verticesSphere[1] = { {vec.x,-vec.y,vec.z},{},{1.0f,0.0f} };//上

		for (int i = 0; i < 36; i++)//横
		{
			worldMat.rot.y = (float)i * AngletoRadi(360.0f / 35.0f);


			for (int j = 0; j < 34; j++)//縦
			{
				worldMat.rot.x = ((float)(j + 1) * (pi / 35.0f));
				worldMat.SetWorld();
				vec = { 0,-5.0f,0 };
				Vec3xM4(vec, worldMat.matWorld, false);

				int p = i * 34 + j;
				verticesSphere[(2) + i * 34 + j] = { {vec.x,vec.y,vec.z},{},{1.0f,0.0f} };
			}
		}

		int count = 0;
		int count2 = 0;
		int count3 = 0;
		int count4 = 0;
		bool isLast = false;
		//インデックス
		for (int i = 0; i < _countof(indicesSphere); i++)
		{
			if (i % (PAPA * 35) == 0 && i != 0)//最後の縦の列
			{
				isLast = true;
			}

			if (i % PAPA == 0 || i % (PAPA * (count + 1) - 3) == 0)
			{

			}
			else if (1)//一番下か上じゃなければ
			{
				if (count2 % 2 == 0)
				{
					if (isLast)
					{
						indicesSphere[i] = 2 + 34 * count + (count3);
						indicesSphere[i + 1] = 2 + 0 + (count3);//一周してきたので一番最初の列を使う
						indicesSphere[i + 2] = 2 + 34 * count + (count3 + 1);

						count3++;
						i += 2;
					}
					else
					{
						indicesSphere[i] = 2 + 34 * count + (count3);
						indicesSphere[i + 1] = 2 + 34 * (count + 1) + (count3);
						indicesSphere[i + 2] = 2 + 34 * count + (count3 + 1);

						count3++;
						i += 2;
					}
				}
				else if (count2 % 2 == 1)
				{
					if (isLast)
					{
						indicesSphere[i] = 2 + 0 + (count4 + 1);//一周してきたので一番最初の列を使う
						indicesSphere[i + 1] = 2 + 34 * count + (count4 + 1);
						indicesSphere[i + 2] = 2 + 0 + (count4);//一周してきたので一番最初の列を使う

						count4++;
						i += 2;
					}
					else
					{
						indicesSphere[i] = 2 + 34 * (count + 1) + (count4 + 1);
						indicesSphere[i + 1] = 2 + 34 * count + (count4 + 1);
						indicesSphere[i + 2] = 2 + 34 * (count + 1) + (count4);

						count4++;
						i += 2;
					}
				}

				count2++;
			}
			if (i % PAPA == 0 || i % (PAPA * (count + 1) - 3) == 0)
			{
				if (i % PAPA == 0)//一番下の三角形
				{
					if (isLast)
					{
						indicesSphere[i] = 0;
						indicesSphere[i + 1] = 2 + 0;
						indicesSphere[i + 2] = 2 + 34 * count;

						i += 2;
					}
					else
					{
						indicesSphere[i] = 2 + 34 * (count + 1);
						indicesSphere[i + 1] = 2 + 34 * count;
						indicesSphere[i + 2] = 0;

						i += 2;
					}
				}
				else if (i % (PAPA * (count + 1) - 3) == 0)//一番上の三角形
				{
					if (isLast)
					{
						indicesSphere[i] = 1 + 34 * (count + 1);
						indicesSphere[i + 1] = 35;
						indicesSphere[i + 2] = 1;

						i += 2;

						count++;
						count2 = 0;
						count3 = 0;
						count4 = 0;
					}
					else
					{
						indicesSphere[i] = 1 + 34 * (count + 1);
						indicesSphere[i + 1] = 1 + 34 * (count + 2);
						indicesSphere[i + 2] = 1;

						i += 2;

						count++;
						count2 = 0;
						count3 = 0;
						count4 = 0;
					}
				}
			}
		}

		UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indicesSphere));

		//リソース設定
		ResourceProperties(TextureManager::GetInstance().resDesc, sizeIB);
		//インデックスバッファの作成
		ID3D12Resource* indexBuff = nullptr;//GPU側のメモリ
		BuffProperties(heapProp, TextureManager::GetInstance().resDesc, &indexBuff);
		//インデックスバッファをマッピング
		uint16_t* indexMap = nullptr;
		Directx::GetInstance().result = indexBuff->Map(0, nullptr, (void**)&indexMap);
		//全インデックスに対して
		for (int i = 0; i < _countof(indicesSphere); i++)
		{
			indexMap[i] = indicesSphere[i];//インデックスをコピー

		}
		//マッピングを解除
		indexBuff->Unmap(0, nullptr);

		//インデックスバッファビューの作成
		ibViewSphere.BufferLocation = indexBuff->GetGPUVirtualAddress();
		ibViewSphere.Format = DXGI_FORMAT_R16_UINT;
		ibViewSphere.SizeInBytes = sizeIB;


		//06_03
		/*if (indexNum == SPHERE)*/
		{
			for (int i = 0; i < _countof(indicesSphere) / 3; i++)
			{//三角形一つごとに計算
				//三角形のインデックスを取り出して、一時的な変数に入れる
				unsigned short index0 = indicesSphere[i * 3 + 0];
				unsigned short index1 = indicesSphere[i * 3 + 1];
				unsigned short index2 = indicesSphere[i * 3 + 2];
				//三角形を構成する頂点座標をベクトルに代入
				XMVECTOR p0 = XMLoadFloat3(&verticesSphere[index0].pos);
				XMVECTOR p1 = XMLoadFloat3(&verticesSphere[index1].pos);
				XMVECTOR p2 = XMLoadFloat3(&verticesSphere[index2].pos);
				//p0->p1ベクトル、p0->p2ベクトルを計算
				XMVECTOR v1 = XMVectorSubtract(p1, p0);
				XMVECTOR v2 = XMVectorSubtract(p2, p0);
				//外積（垂直なベクトル）
				XMVECTOR normal = XMVector3Cross(XMVector3Normalize(v1), XMVector3Normalize(v2));
				//求めた法線を頂点データに代入
				XMStoreFloat3(&verticesSphere[index0].normal, normal);
				XMStoreFloat3(&verticesSphere[index1].normal, normal);
				XMStoreFloat3(&verticesSphere[index2].normal, normal);
			}
		}
	}
}



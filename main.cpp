#include "WinApp.h"
#include "Input.h"
#include "DirectXBasis.h"

#include <DirectXMath.h>
#include <DirectXTex.h>

#include <string>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#include <d3dcompiler.h>//シェーダ用コンパイラ

#include "Struct.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#pragma comment(lib, "d3dcompiler.lib")//シェーダ用コンパイラ

using namespace DirectX;
using namespace Microsoft::WRL;

//定数バッファ用データ構造体(マテリアル)
struct ConstBufferDataMaterial
{
	XMFLOAT4 color; //色(RGBA)
};

//資料05-02で追加
#pragma region 3D変換行列
//定数バッファ用データ構造体(3D変換行列)
struct ConstBufferDataTransform {
	XMMATRIX mat; //3D変換行列
};

//3Dオブジェクト型
struct Object3d
{
	//定数バッファ(行列用)
	ComPtr<ID3D12Resource> constBuffTransform = {};

	//定数バッファマップ(行列用)
	ConstBufferDataTransform* constMapTransform = {};

	//アフィン変換情報
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 position = { 0,0,0 };

	//ワールド変換行列
	XMMATRIX matWorld = {};

	//親オブジェクトへのポインタ
	Object3d* parent = nullptr;
};

struct TextureData
{
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	ScratchImage mipChine{};

	D3D12_HEAP_PROPERTIES textureHeapProp{};
	D3D12_RESOURCE_DESC textureResourceDesc{};

	//テクスチャバッファの生成
	ComPtr<ID3D12Resource> texBuff = nullptr;
};

//3Dオブジェクトの初期化
void InitializeObject3d(Object3d* object, ID3D12Device* device)
{
	HRESULT result;

#pragma region constMapTransfrom関連

	//ヒープ設定
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
											//リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff; //256バイトアラインメント
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = device->CreateCommittedResource(
		&heapProp, //ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&object->constBuffTransform));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	result = object->constBuffTransform->Map(0, nullptr,
		(void**)&object->constMapTransform); //マッピング
	assert(SUCCEEDED(result));

#pragma endregion
}

void InitializeTexture(TextureData* textureData, const wchar_t* szFile)
{
	HRESULT result;

	//WICテクスチャのロード
	result = LoadFromWICFile(
		szFile,
		WIC_FLAGS_NONE,
		&textureData->metadata,
		textureData->scratchImg);

	//ミップマップ生成
	result = GenerateMipMaps(
		textureData->scratchImg.GetImages(),
		textureData->scratchImg.GetImageCount(),
		textureData->scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, textureData->mipChine);
	if (SUCCEEDED(result)) {
		textureData->scratchImg = std::move(textureData->mipChine);
		textureData->metadata = textureData->scratchImg.GetMetadata();
	}
	//読み込んだでデイヒューズテクスチャをSRGBとして扱う
	textureData->metadata.format = MakeSRGB(textureData->metadata.format);
};

void TransferTextureBuffer(TextureData* textureData, ID3D12Device* device)
{
	HRESULT result;

	//ヒープ設定
	//D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureData->textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureData->textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureData->textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//リソース設定
	//D3D12_RESOURCE_DESC textureResourceDesc{};
	textureData->textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureData->textureResourceDesc.Format = textureData->metadata.format;
	textureData->textureResourceDesc.Width = textureData->metadata.width; //幅
	textureData->textureResourceDesc.Height = (UINT)textureData->metadata.height; //高さ
	textureData->textureResourceDesc.DepthOrArraySize = (UINT16)textureData->metadata.arraySize;
	textureData->textureResourceDesc.MipLevels = (UINT16)textureData->metadata.mipLevels;
	textureData->textureResourceDesc.SampleDesc.Count = 1;

	////テクスチャバッファの生成
	//ID3D12Resource* texBuff = nullptr;

	result = device->CreateCommittedResource(
		&textureData->textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureData->textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureData->texBuff));

	//全ミップマップについて
	for (size_t i = 0; i < textureData->metadata.mipLevels; i++)
	{
		//ミップマップレベルを指定してイメージを取得
		const Image* img = textureData->scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		result = textureData->texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,//全領域へコピー
			img->pixels,//元データアドレス
			(UINT)img->rowPitch,//1ラインサイズ
			(UINT)img->slicePitch//一枚サイズ
		);
		assert(SUCCEEDED(result));
	}
}

//3Dオブジェクトの初期化処理の呼び出し
void SetIntializeObject3ds(Object3d* object, ID3D12Device* device, int objectNum)
{
	//初期化
	InitializeObject3d(object, device);

	//ここから↓は親子構造のサンプル
	//先頭以外なら
	if (objectNum > 0) {
		//1つ前のオブジェクトを親オブジェクトとする
		object->parent = &object[objectNum - 1];
		//親オブジェクトの9割の大きさ
		object->scale = { 0.9f,0.9f,0.9f };
		//親オブジェクトに対してZ軸まわりに30度回転
		object->rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
		//親オブジェクトに対してZ方向に-8.0ずらす
		object->position = { 0.0f,0.0f,-8.0f };
	}
}

//オブジェクト更新処理
void UpdateObject3d(Object3d* object, XMMATRIX& matView, XMMATRIX& matProjection)
{
	XMMATRIX matScale, matRot, matTrans;

	//スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(object->scale.x, object->scale.y, object->scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(object->rotation.z);
	matRot *= XMMatrixRotationX(object->rotation.x);
	matRot *= XMMatrixRotationY(object->rotation.y);
	matTrans = XMMatrixTranslation(
		object->position.x, object->position.y, object->position.z);

	//ワールド行列の合成
	object->matWorld = XMMatrixIdentity();	//変形リセット
	object->matWorld *= matScale;	//ワールド行列のスケーリングを反映
	object->matWorld *= matRot;	//ワールド行列に回転を反映
	object->matWorld *= matTrans;	//ワールド行列に平行移動を反映

	//親オブジェクトがあれば
	if (object->parent != nullptr) {
		//親オブジェクトのワールド行列を掛ける
		object->matWorld *= object->parent->matWorld;
	}

	//定数バッファへデータ転送
	object->constMapTransform->mat = object->matWorld * matView * matProjection;
}

void DrawObject3d(Object3d* object, ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView,
	D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices) {
	//頂点バッファの設定
	commandList->IASetVertexBuffers(0, 1, &vbView);
	//インデックスバッファの設定
	commandList->IASetIndexBuffer(&ibView);
	//定数バッファビュー(CBV)の設定コマンド
	commandList->SetGraphicsRootConstantBufferView(2, object->constBuffTransform->GetGPUVirtualAddress());

	//描画コマンド
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
}

//座標操作
void UpdateObjectPosition(Object3d* object, Input* input) {
	if (input->ifKeyPress(DIK_UP)) { object->position.y += 1.0f; }
	else if (input->ifKeyPress(DIK_DOWN)) { object->position.y -= 1.0f; }
	if (input->ifKeyPress(DIK_RIGHT)) { object->position.x += 1.0f; }
	else if (input->ifKeyPress(DIK_LEFT)) { object->position.x -= 1.0f; }
}
//回転操作
void UpdateObjectRotation(Object3d* object, Input* input) {
	if (input->ifKeyPress(DIK_Q)) { object->rotation.z += 0.1f; }
	else if (input->ifKeyPress(DIK_E)) { object->rotation.z -= 0.1f; }
}
//オブジェクト操作
void UpdateObjectControll(Object3d* object, Input* input) {
	UpdateObjectRotation(object, input);
	UpdateObjectPosition(object, input);
	if (input->ifKeyTrigger(DIK_RETURN)) { object->position.y -= 6.0f; }

	if (input->ifKeyRelease(DIK_RETURN)) { object->position.y += 20.0f; }
}

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//------WindowsAPI初期化処理 ここから------
	HRESULT result;
	const float PI = 3.1415926535f;

	//ポインタ
	WinApp* winApp = nullptr;
	//WinApp初期化
	winApp = new WinApp();
	winApp->Initialize();

	//------WindowsAPI初期化処理 ここまで------

	//------DirectX初期化処理 ここから------
	//ポインタ
	Input* input = nullptr;
	//入力の初期化
	input = new Input();
	input->Initialize(winApp);

	//ポインタ
	DirectXBasis* dXBas = nullptr;
	//DirectXBasis初期化
	dXBas = new DirectXBasis();
	dXBas->Initialize(winApp);
	//------DirectX初期化処理 ここまで------

	//------描画初期化処理 ここから------
#pragma region
	float angle = 0.0f; //カメラの回転角

	//拡縮倍率
	XMFLOAT3 scale;
	//回転角
	XMFLOAT3 rotation;
	//座標
	XMFLOAT3 position;
	//座標
	//XMFLOAT3 position1;

	//拡縮倍率
	scale = { 1.0f,1.0f,1.0f };
	//回転角
	rotation = { 00.0f,00.0f,00.0f };
	//座標
	position = { 0.0f,0.0f,0.0f };
	//position1 = { -20.0f,0.0f,0.0f };

	//頂点データ構造体
	struct Vertex
	{
		XMFLOAT3 pos;		//xyz座標
		XMFLOAT3 normal;	//法線ベクトル
		XMFLOAT2 uv;		//uv座標
	};

	////頂点データ
	//Vertex vertices[] =
	//{
	//	//x		 y		z		u	  v
	//	{{-50.0f, -50.0f, 0.0f}, {0.0f, 1.0f}},//左下
	//	{{-50.0f,  50.0f, 0.0f}, {0.0f, 0.0f}},//左上
	//	{{ 50.0f, -50.0f, 0.0f}, {1.0f, 1.0f}},//右下
	//	{{ 50.0f,  50.0f, 0.0f}, {1.0f, 0.0f}},//右上
	//};

	//頂点データ
	Vertex vertices[] =
	{
		//x		 y		z		法線	u	  v
		//前
		{{-5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//左下
		{{-5.0f,  5.0f, -5.0f},	{},		{0.0f, 0.0f}},//左上
		{{ 5.0f, -5.0f, -5.0f},	{},		{1.0f, 1.0f}},//右下
		{{ 5.0f,  5.0f, -5.0f},	{},		{1.0f, 0.0f}},//右上

		//後ろ				 
		{{ 5.0f, -5.0f,  5.0f},	{},		{1.0f, 1.0f}},//右下
		{{ 5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//右上
		{{-5.0f, -5.0f,  5.0f},	{},		{0.0f, 1.0f}},//左下
		{{-5.0f,  5.0f,  5.0f},	{},		{0.0f, 0.0f}},//左上

		//左							
		{{-5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//左下
		{{-5.0f, -5.0f,  5.0f},	{},		{0.0f, 0.0f}},//左上
		{{-5.0f,  5.0f, -5.0f},	{},		{1.0f, 1.0f}},//右下
		{{-5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//右上

		//右							
		{{ 5.0f,  5.0f, -5.0f},	{},		{1.0f, 1.0f}},//右下
		{{ 5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//右上
		{{ 5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//左下
		{{ 5.0f, -5.0f,  5.0f},	{},		{0.0f, 0.0f}},//左上

		//下							
		{{-5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//左下
		{{ 5.0f, -5.0f, -5.0f},	{},		{0.0f, 0.0f}},//左上
		{{-5.0f, -5.0f,  5.0f},	{},		{1.0f, 1.0f}},//右下
		{{ 5.0f, -5.0f,  5.0f},	{},		{1.0f, 0.0f}},//右上

		//上							
		{{-5.0f,  5.0f,  5.0f},	{},		{1.0f, 1.0f}},//右下
		{{ 5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//右上
		{{-5.0f,  5.0f, -5.0f},	{},		{0.0f, 1.0f}},//左下
		{{ 5.0f,  5.0f, -5.0f},	{},		{0.0f, 0.0f}},//左上
	};

	//インデックスデータ
	unsigned short indices[] =
	{
		//前
		0,1,2,//一つ目
		2,1,3,//二つ目
		//後ろ
		4,5,6,//三つ目
		6,5,7,//四つ目
		//左
		8,9,10,//一つ目
		10,9,11,//二つ目
		//右
		12,13,14,
		14,13,15,
		//下
		16,17,18,//一つ目
		18,17,19,//二つ目
		//上
		20,21,22,
		22,21,23,
	};

	bool ifOneTextureNum = true;

	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

	//頂点バッファの設定
	//ヒープ設定
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUの転送用
										   //リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;//頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//頂点バッファの生成
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	result = dXBas->GetDevice()->CreateCommittedResource(
		&heapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//GPU上のバッファに対応仮想メモリ(メインメモリ上)を取得
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	/* verticesに記入 */

	//全頂点に対して
	for (int i = 0; i < _countof(vertices); i++)
	{
		vertMap[i] = vertices[i];//座標をコピー
	}

	//繋がりを解除
	vertBuff->Unmap(0, nullptr);

	//頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//GPU仮想アドレス
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView.SizeInBytes = sizeVB;
	//頂点１つ分のデータサイズ
	vbView.StrideInBytes = sizeof(vertices[0]);

	ComPtr<ID3DBlob> vsBlob = nullptr;//頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob = nullptr;//ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob = nullptr;//エラーオブジェクト

	//頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicVS.hlsl",//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
		"main", "vs_5_0",//エントリーポイント名、シェーダ―モデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
		0,
		&vsBlob, &errorBlob);

	//エラーなら
	if (FAILED(result)) {
		//errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicPS.hlsl",//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
		"main", "ps_5_0",//エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
		0,
		&psBlob, &errorBlob);

	//エラーなら
	if (FAILED(result)) {
		//errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{
			//xyz座標
			"POSITION",									//セマンティック名
			0,											//同じセマンティック名が複数あるときに使うインデックス(0でよい)
			DXGI_FORMAT_R32G32B32_FLOAT,				//要素数とビット数を表す　(XYZの3つでfloat型なのでR32G32B32_FLOAT
			0,											//入力スロットインデックス(0でよい)
			D3D12_APPEND_ALIGNED_ELEMENT,				//データのオフセット値　(D3D12_APPEND_ALIGNED_ELEMENTだと自動設定)
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	//入力データ種別　(標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DAT
			0											//一度に描画するインスタンス数(0でよい)
		},//(1行で書いた方が見やすいかも)

		{
			//法線ベクトル
			"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},

		//座標以外に 色、テクスチャUIなどを渡す場合はさらに続ける
		{
			//UV座標
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0,
		},
		//{/*...*/},
	};

	//グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//シェーダーの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	//サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//標準設定
	//pipelineDesc.SampleMask = UINT_MAX;

	//ラスタライザの設定
	//背面カリングも設定する
	//pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//カリングしない
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;//背面カリングする
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true;//深度クリッピングを有効に

	//ブレンドステート
	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
		= D3D12_COLOR_WRITE_ENABLE_ALL;//RGB全てのチャネルを描画

	//レンダ―ターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//アルファ値共通設定
	blenddesc.BlendEnable = false; // ブレンド有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; //ブレンドを有効にする
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; //加算
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; //デストの値を 0%使う　

	//加算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //加算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE; //ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE; //デストの値を100%使う

	//減算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT; //減算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE; //ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE; //デストの値を100%使う

	//色反転
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //加算
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR; //1.0f-デストから－の値
	//blenddesc.DestBlend = D3D12_BLEND_ZERO; //使わない

	//半透明合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //加算
	blenddesc.SrcBlend = D3D12_BLEND_ONE; //ソースの値をアルファ値
	blenddesc.DestBlend = D3D12_BLEND_ONE; //1.0f-ソースのアルファ値

	//頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状設定
	pipelineDesc.PrimitiveTopologyType
		= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

#pragma region 深度テストの設定
	//デプスステンシルステートの設定
	pipelineDesc.DepthStencilState.DepthEnable = true;
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	pipelineDesc.DSVFormat = DXGI_FORMAT_R32_FLOAT;
#pragma endregion

	//その他の設定
	pipelineDesc.NumRenderTargets = 1;//描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0～255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1;//1ピクセルにつき1回サンプリング

	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature;

	//デスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;//一度の描画に使うテクスチャが一枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;//テクスチャレジスタ番号0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//デスクリプタテーブルの設定
	D3D12_DESCRIPTOR_RANGE descRange{};
	descRange.NumDescriptors = 1;//定数は一つ
	descRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV; //種別は定数
	descRange.BaseShaderRegister = 0; //0番スロットから
	descRange.OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	////ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	//定数バッファ0番
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//定数バッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0;					//定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える

	//テクスチャレジスタ0番
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//定数バッファビュー
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;					//定数バッファ番号
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;						//デフォルト値
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える

	//定数バッファ1番
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[2].Descriptor.ShaderRegister = 1;					//定数バッファ番号
	rootParams[2].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える

	//テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams; //ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams); //ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	//ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob);
	assert(SUCCEEDED(result));

	result = dXBas->GetDevice()->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));

	//パイプラインにルートシグネイチャをセット
	pipelineDesc.pRootSignature = rootSignature.Get();

	//パイプラインステートの生成
	ComPtr<ID3D12PipelineState> pipelineState = nullptr;
	result = dXBas->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc,
		IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

	ComPtr<ID3D12Resource> constBuffMaterial = nullptr;

#pragma endregion

#pragma region

#pragma region constMapMaterial関連

	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbheapprop{};
	cbheapprop.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbresdesc{};
	cbresdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbresdesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff; //256バイトアラインメント
	cbresdesc.Height = 1;
	cbresdesc.DepthOrArraySize = 1;
	cbresdesc.MipLevels = 1;
	cbresdesc.SampleDesc.Count = 1;
	cbresdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = dXBas->GetDevice()->CreateCommittedResource(
		&cbheapprop, //ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbresdesc, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial); //マッピング

	// 値を書き込むと自動的に転送される
	constMapMaterial->color = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f); //RGBAで半透明の赤

	//マッピング解除
	constBuffMaterial->Unmap(0, nullptr);
	assert(SUCCEEDED(result));

#pragma endregion

#pragma region constMapTransfrom関連

	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff; //256バイトアラインメント
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

#pragma region 三次元オブジェクトの構造化

	//3Dオブジェクトの数
	const size_t kObjectCount = 1;
	//3Dオブジェクトの配列
	Object3d object3ds[kObjectCount];

	XMFLOAT3 rndScale;
	XMFLOAT3 rndRota;
	XMFLOAT3 rndPos;

	//配列内の全オブジェクトに対して
	for (int i = 0; i < _countof(object3ds); i++)
	{
		SetIntializeObject3ds(&object3ds[i], dXBas->GetDevice(), i);

		//初期化
		InitializeObject3d(&object3ds[i], dXBas->GetDevice());

		//ここから↓は親子構造のサンプル
		//先頭以外なら
		if (i > 0) {
			rndScale = {
				0.7f,
				0.7f,
				0.7f,
			};

			rndRota = {
				XMConvertToRadians(static_cast<float>(rand() % 90)),
				XMConvertToRadians(static_cast<float>(rand() % 90)),
				XMConvertToRadians(static_cast<float>(rand() % 90)),
			};

			rndPos = {
				static_cast<float>(rand() % 60 - 30),
				static_cast<float>(rand() % 60 - 30),
				static_cast<float>(rand() % 60 - 30),
			};

			object3ds[i].scale = rndScale;

			object3ds[i].rotation = rndRota;

			object3ds[i].position = rndPos;

			//1つ前のオブジェクトを親オブジェクトとする
			//object3ds[i].parent = &object3ds[i - 1];
			////親オブジェクトの9割の大きさ
			//object3ds[i].scale = { 0.9f,0.9f,0.9f };
			////親オブジェクトに対してZ軸まわりに30度回転
			//object3ds[i].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f)};
			////親オブジェクトに対してZ方向に-8.0ずらす
			//object3ds[i].position = { 0.0f,0.0f,-8.0f };
		}

	}

#pragma endregion

#pragma region 単位行列で埋めた後
#pragma region 平行投影行列の計算

	////DirectXMathで用意されている関数に置き換え
	//constMapTransform0->mat = XMMatrixOrthographicOffCenterLH(
	//	0.0f, window_width,//左端、右端
	//	window_height, 0.0f,//下端、上端
	//	0.0f, 1.0f);//前端、奥端
#pragma endregion

#pragma region 投資投影変換行列の計算

	XMMATRIX matProjection =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(45.0f),//上下画角45度
			(float)WinApp::WinWidth / WinApp::WinHeight,//アスペクト比(画面横幅/画面縦幅)
			0.1f, 1000.0f
		);//前端、奥端

#pragma region ビュー行列の作成
	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -100);	//視点座標
	XMFLOAT3 target(0, 0, 0);	//注視点座標
	XMFLOAT3 up(0, 1, 0);		//上方向ベクトル
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye),
		XMLoadFloat3(&target), XMLoadFloat3(&up));

#pragma endregion

#pragma endregion

#pragma region ワールド変換行列

	//XMMATRIX matWorld;
	//matWorld = XMMatrixIdentity();

	//XMMATRIX matScale; //スケーリング行列

	//XMMATRIX matRot; //回転行列
	//matRot = XMMatrixIdentity();

	//XMMATRIX matTrans; //平行移動行列
	//matTrans = XMMatrixTranslation(0, 0, 0);

	//matWorld *= matTrans; //ワールド行列に平行移動を反映

#pragma endregion

#pragma endregion

	// インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

	// リソース設定
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB; // インデックス情報が入る分のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//インデックスバッファの生成
	ComPtr<ID3D12Resource> indexBuff = nullptr;
	result = dXBas->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	//　インデックスバッファをマッピング
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	// 全インデックスに対して
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i]; //インデックスをコピー
	}
	//マッピング解除
	indexBuff->Unmap(0, nullptr);

	//インデックスバッファビューの作成
	D3D12_INDEX_BUFFER_VIEW ibView{};
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	const int kTextureCount = 2;
	TextureData textureDatas[kTextureCount] = { 0 };

	const wchar_t* texImgs[kTextureCount] =
	{
		L"Resources/texture.jpg",
		L"Resources/reimu.png",
	};

	for (size_t i = 0; i < _countof(textureDatas); i++)
	{
		InitializeTexture(&textureDatas[i], texImgs[i]);
	}

	for (size_t i = 0; i < _countof(textureDatas); i++)
	{
		TransferTextureBuffer(&textureDatas[i], dXBas->GetDevice());
	}

	//元データ開放
	//delete[] imageData;

	//SRVの最大個数
	const size_t kMaxSRVCount = 2056;

	//デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	//設定を基にSRV用デスクリプタヒープを生成
	ID3D12DescriptorHeap* srvHeap = nullptr;
	result = dXBas->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));

	//SRVヒープの先頭ハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();

	//シェーダリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	srvDesc.Format = resDesc.Format;//RGBA float
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = resDesc.MipLevels;

	//ハンドルの指す位置にシェーダーリソースビュー作成
	dXBas->GetDevice()->CreateShaderResourceView(textureDatas[0].texBuff.Get(), &srvDesc, srvHandle);

#pragma region テクスチャの差し替えで追記
	//サイズ変更
	UINT incrementSize = dXBas->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvHandle.ptr += incrementSize;

	//2枚目用
	//シェーダリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};//設定構造体
	srvDesc2.Format = textureDatas[1].textureResourceDesc.Format;//RGBA float
	srvDesc2.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc2.Texture2D.MipLevels = textureDatas[1].textureResourceDesc.MipLevels;

	//ハンドルの指す位置にシェーダーリソースビュー作成
	dXBas->GetDevice()->CreateShaderResourceView(textureDatas[1].texBuff.Get(), &srvDesc2, srvHandle);

#pragma endregion

	//CBV,SRV,UAVの1個分のサイズを取得
	UINT descriptorSize = dXBas->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//SRVヒープの先頭ハンドルを取得
	//D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	//ハンドルを一つ進める(SRVの位置)
	srvHandle.ptr += descriptorSize * 1;

	//CBV(コンスタントバッファビュー)の設定
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	//cbvDescの値設定(省略)
	dXBas->GetDevice()->CreateConstantBufferView(&cbvDesc, srvHandle);

#pragma endregion

#pragma endregion
	//------描画初期化処理 ここまで------
	//ゲームループ
	while (true) {
		//入力更新
		input->Update();



#pragma region ターゲットの周りを回るカメラ
		if (input->ifKeyPress(DIK_D) || input->ifKeyPress(DIK_A))
		{
			if (input->ifKeyPress(DIK_D)) { angle += XMConvertToRadians(1.0f); }
			else if (input->ifKeyPress(DIK_A)) { angle -= XMConvertToRadians(1.0f); }

			//angleラジアンだけY軸周りに回転、半径は-100
			eye.x = -100 * sinf(angle);
			eye.z = -100 * cosf(angle);
			matView = XMMatrixLookAtLH(XMLoadFloat3(&eye),
				XMLoadFloat3(&target), XMLoadFloat3(&up));

			object3ds[0].constMapTransform->mat = matView * matProjection;
		}
#pragma endregion

#pragma region 連続移動

		for (size_t i = 0; i < _countof(object3ds); i++)
		{
			UpdateObject3d(&object3ds[i], matView, matProjection);

		}

		UpdateObjectControll(&object3ds[0], input);



		//描画の準備
		dXBas->PrepareDraw();

#pragma region 法線を計算
		for (int i = 0; i < _countof(indices) / 3; i++)
		{
			//三角形１つごとに計算していく
			//三角形のインデックスを取り出して、一時的な変数に入れる
			unsigned short index0 = indices[i * 3 + 0];
			unsigned short index1 = indices[i * 3 + 1];
			unsigned short index2 = indices[i * 3 + 2];

			//三角形を構成する頂点座標をベクトルに代入
			XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
			XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
			XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);

			//p0->p1ベクトル、 p0->p2ベクトルを計算 (ベクトルの減算)
			XMVECTOR v1 = XMVectorSubtract(p1, p0);
			XMVECTOR v2 = XMVectorSubtract(p2, p0);

			//外積は両方から垂直なベクトル
			XMVECTOR normal = XMVector3Cross(v1, v2);

			//正規化 (長さを1にする)
			normal = XMVector3Normalize(normal);

			//求めた法線を頂点データに代入
			XMStoreFloat3(&vertices[index0].normal, normal);
			XMStoreFloat3(&vertices[index1].normal, normal);
			XMStoreFloat3(&vertices[index2].normal, normal);
		}
#pragma endregion

#pragma region 転送
		//全頂点に対して
		for (int i = 0; i < _countof(vertices); i++)
		{
			vertMap[i] = vertices[i];//座標をコピー
		}
#pragma endregion

		//パイプラインステートとルートシグネチャの設定コマンド
		dXBas->GetCommandList()->SetPipelineState(pipelineState.Get());
		dXBas->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

		//プリミティブ形状の設定コマンド
		dXBas->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//三角形リスト

		//頂点バッファビューの設定コマンド
		dXBas->GetCommandList()->IASetVertexBuffers(0, 1, &vbView);

		//定数バッファビュー(CBV)の設定コマンド
		dXBas->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

		//SRVヒープの設定コマンド
		dXBas->GetCommandList()->SetDescriptorHeaps(1, &srvHeap);

		//SRVヒープの先頭ハンドルを取得(SRVを指しているはず)
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
		//SRVヒープの先頭にあるSRVをルートパラメータ1番に設定
		dXBas->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

		if (ifOneTextureNum == false)
		{
			//2枚目を指し示すようにしたSRVのハンドルをルートパラメータに設定
			srvGpuHandle.ptr += incrementSize;
			dXBas->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
		}

		//インデックスバッファビューの設定コマンド
		dXBas->GetCommandList()->IASetIndexBuffer(&ibView);

		//全オブジェクトについて処理
		for (int i = 0; i < _countof(object3ds); i++)
		{
			DrawObject3d(&object3ds[i], dXBas->GetCommandList(), vbView, ibView, _countof(indices));
		}

		//描画後処理
		dXBas->PostDraw();

		//windowsのメッセージ処理
		if (winApp->ProcessMessage()) {
			//ループを抜ける
			break;
		}
	}

	//WindowsAPI終了処理
	winApp->Finalize();

	//入力の解放
	delete input;
	input = nullptr;

	//基盤の解放
	delete dXBas;
	dXBas = nullptr;

	//WinAppの解放
	delete winApp;
	winApp = nullptr;

	return 0;
}
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

//静的メンバ変数
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
	//nullチェック
	assert(directX_);
	//メンバに渡す
	directX = directX_;

	//モデルクラスにデバイスのインスタンスを渡す
	Model::SetDevice(directX->GetDevice());
	//3D用パイプラインステート生成
	CreatePipeline3D();
}

void Object3d::BeginDraw(const Camera& camera)
{
	//パイプラインステートの設定
	directX->GetCommandList()->SetPipelineState(pipelineState.Get());
	//ルートシグネチャの設定
	directX->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	//プリミティブ形状の設定
	directX->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//3番定数バッファビューにカメラの定数バッファを設定
	directX->GetCommandList()->SetGraphicsRootConstantBufferView(3, camera.constBuff->GetGPUVirtualAddress());
}

void Object3d::Initialize()
{
	ID3D12Device* device = directX->GetDevice();

	HRESULT result;
	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{  };
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferData) + 0xff) & ~0xff;	//256バイトアラインメント
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
		IID_PPV_ARGS(&constBuffB0));
	assert(SUCCEEDED(result));
	//定数バッファのマッピング
	result = constBuffB0->Map(0, nullptr, (void**)&constMap);//マッピング
	assert(SUCCEEDED(result));


	XMMATRIX matrix = XMMatrixIdentity();
	constMap->mat = matrix;


}

void Object3d::Update()
{
	XMMATRIX matScale, matRot, matTrans;

	//各行列計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(rotation.z);	//Z軸周りに回転
	matRot *= XMMatrixRotationX(rotation.x);	//X軸周りに回転
	matRot *= XMMatrixRotationY(rotation.y);	//Y軸周りに回転
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	matWorld = XMMatrixIdentity();
	matWorld = XMMatrixIdentity();

	matWorld *= matScale;	//スケーリングを反映
	matWorld *= matRot;	//回転を反映
	matWorld *= matTrans;	//平行移動を反映

	if (parent != nullptr) {
		matWorld *= parent->matWorld;
	}

	//定数バッファに転送
	//constMap->mat = matWorld * matView * matProjection;
	constMap->mat = matWorld;
}

void Object3d::Draw()
{
	ID3D12GraphicsCommandList* commandList = directX->GetCommandList();
	//h定数バッファビュー(CBV)の設定コマンド
	commandList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	//モデルデータの描画用コマンドのまとまり
	model->Draw(commandList, 2);
}

void Object3d::CreatePipeline3D()
{
	HRESULT result;

	ID3D12Device* dev = directX->GetDevice();

	ComPtr<ID3DBlob> vsBlob;		// 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;		// ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob;	// エラーオブジェクト

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/ObjVertexShader.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);
	// エラーなら
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}


	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/ObjPixelShader.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);
	// エラーなら
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//パイプライン設定
	GpPipeline pipeline3D;
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
	inputLayout.push_back(
		{
		"POSITION",										//セマンティック名
		0,												//同じセマンティック名が複数ある時に使うインデックス（0でよい）
		DXGI_FORMAT_R32G32B32_FLOAT,					//要素数とビット数を表す（XYZの3つでfloat型なのでR32G32B32_FLOAT）
		0,												//入力スロットインデックス(0でよい)
		D3D12_APPEND_ALIGNED_ELEMENT,					//データのオフセット値(D3D12_APPEND_ALIGNED_ELEMENTだと自動設定)
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		//入力データ種別(標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA)
		0												//一度に描画するインスタンス数(0でよい)
		});

	inputLayout.push_back(
		{//法線ベクトル
		"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		});
	inputLayout.push_back({//uv座標
		"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		});

	pipeline3D.SetPipeline(vsBlob.Get(), psBlob.Get(), inputLayout);

	//デスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;		//一度の描画に使うテクスチャが1枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;	//テクスチャレジスタ0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParams[4] = {};
	//定数バッファ0番
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//定数バッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0;					//定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//すべてのシェーダから見える
	//テクスチャレジスタ0番
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//種類
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;			//デスクリプタレンジ
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;						//デスクリプタレンジ数
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//すべてのシェーダから見える
	//定数バッファ1番
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[2].Descriptor.ShaderRegister = 1;					//デスクリプタレンジ
	rootParams[2].Descriptor.RegisterSpace = 0;						//デスクリプタレンジ数
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//すべてのシェーダから見えるバッファ
	//定数バッファ2番
	rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[3].Descriptor.ShaderRegister = 2;					//デスクリプタレンジ
	rootParams[3].Descriptor.RegisterSpace = 0;						//デスクリプタレンジ数
	rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//すべてのシェーダから見えるバッファ

	//テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;				//横繰り返し（タイリング）
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;				//縦繰り返し（タイリング）
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;				//奥行繰り返し（タイリング）
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	//ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;					//すべてリニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;									//ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;												//ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;			//ピクセルシェーダからのみ使用可能

	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;	//ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams);		//ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;
	// ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));

	//パイプラインにルートシグネチャをセット
	pipeline3D.desc.pRootSignature = rootSignature.Get();

	//パイプラインステートの生成
	pipeline3D.SetPipelineState(dev, pipelineState);
}




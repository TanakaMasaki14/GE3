#pragma once

#include<DirectXMath.h>
#include<d3d12.h>
#include<wrl.h>
#include"Transform.h"
#include"DirectX.h"
#include<vector>
#include<string>
#include"Model.h"
#include"Camera.h"

class Object3d
{
public:

	//サブクラス
		//頂点データ構造体
	struct Vertex {
		DirectX::XMFLOAT3 pos;//xyz座標
		DirectX::XMFLOAT3 normal;//法線ベクトル
		DirectX::XMFLOAT2 uv;//uv座標
	};

	struct ConstBufferData {
		//DirectX::XMFLOAT4 color;
		DirectX::XMMATRIX mat;
	};


	static Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	static ReDirectX* directX;

	//頂点座標情報
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffB0;	//定数バッファマップ（行列用）

	ConstBufferData* constMap = nullptr;

	DirectX::XMFLOAT4 color = { 1,1,1,1 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };	//アフィン変換情報
	DirectX::XMFLOAT3 rotation = { 0,0,0 };	//アフィン変換情報
	DirectX::XMFLOAT3 position = { 0,0,0 };	//アフィン変換情報
	DirectX::XMMATRIX matWorld;	//ワールド変換行列
	Object3d* parent = nullptr;	//親オブジェクトへのポインタ

	Model* model = nullptr;	//モデルデータ

public:
	Object3d();
	~Object3d();

	//静的メンバ関数
	static void StaticInitialize(ReDirectX* directX_);
	static void BeginDraw(const Camera& camera);

	void Initialize();
	void Update();
	void Draw();
	void SetModel(Model* model_) { model = model_; }

private:
	static void CreatePipeline3D();
};


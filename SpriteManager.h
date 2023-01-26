#pragma once
#include<d3d12.h>
#include"DirectX.h"
#include<DirectXMath.h>
#include"Texture.h"



struct VertexPosUv {
	DirectX::XMFLOAT3 pos;	//xyz座標
	DirectX::XMFLOAT2 uv;	//uv座標
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
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;		//パイプラインステート
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;		//ルートシグネチャ

	DirectX::XMMATRIX matProjection;//射影行列


public:
	//初期化
	void Initialize(ReDirectX* directX, int windowWidth, int windowHeight);

	//描画前処理
	void beginDraw();

	/// <summary>
	/// テクスチャコマンドの発行
	/// </summary>
	/// <param name="dev"></param>
	void SetTextureCommand(uint32_t index);

	//指定番号のテクスチャバッファを取得
	ID3D12Resource* GetTextureBuffer(uint32_t index)const { return Texture::texBuffuers[index].Get(); }

private:
	//スプライト用パイプラインステートとルートシグネチャの生成
	void CreatePipeline2D(ID3D12Device* dev);
};


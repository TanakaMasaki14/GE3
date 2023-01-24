#pragma once
#include "Sprite.h"
#include <unordered_map>


struct Vertex
{
	XMFLOAT3 pos;   //xyz座標
	XMFLOAT3 normal;//法線ベクトル
	XMFLOAT2 uv;    //uv座標
};
//頂点データ


class Model
{
private:
	// 頂点法線スムージング用データ
	std::unordered_map<unsigned short, std::vector<unsigned short>> smoothData_;

	//マテリアル
	struct Material
	{
		std::string name;//マテリアル名
		XMFLOAT3 ambient;//アンビエント影響度
		XMFLOAT3 diffuse;//ディフューズ〃
		XMFLOAT3 specular;//スペキュラー〃
		float alpha;//アルファ
		std::string textureFilename;//テクスチャファイル名
		UINT64 textureHandle;

		//コンストラクタ
		Material()
		{
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};

	struct ConstBufferDataMaterial2
	{
		XMFLOAT3 ambient;
		float pad1;       //パディング
		XMFLOAT3 diffuse;
		float pad2;       //パディング
		XMFLOAT3 specular;
		float alpha;
	};

public:
	//モデル用
//頂点データ配列
	std::vector<Vertex> verticesM;
	//頂点インデックス配列
	std::vector<unsigned short> indicesM;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuffM;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuffM;
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbViewM{};
	D3D12_INDEX_BUFFER_VIEW ibViewM{};

	//AL4_02_02
	ComPtr <ID3D12Resource> constBuffMaterial2 = nullptr;
	ConstBufferDataMaterial2* constMapMaterial2 = nullptr;

	//マテリアル
	Material material;


private:
	//OBJファイルから3Dモデルを読み込む
	void LoadFromOBJInternal(const char* folderName, const bool smoothing = false);
	//各種バッファ生成
	void CreateBuffers();

public:
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

public://静的メンバ関数
	//OBJファイルから3Dモデルを読み込む
	//"フォルダ名のみ"を指定すればmtl,obj,textuerを読みこむ（すべて同じ名前であれば）
	static Model* LoadFromOBJ(const char* folderName, const bool smoothing = false);

	/// <summary>
/// 頂点データの数を取得
/// </summary>
/// <returns>頂点データの数</returns>
	inline size_t GetVertexCount() { return verticesM.size(); }

	/// <summary>
/// エッジ平滑化データの追加
/// </summary>
/// <param name="indexPosition">座標インデックス</param>
/// <param name="indexVertex">頂点インデックス</param>
	void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex);

	/// <summary>
	/// 平滑化された頂点法線の計算
	/// </summary>
	void CalculateSmoothedVertexNormals();
};
#pragma once
#include"Vector2.h"
#include"Vector3.h"
#include<string>
#include<vector>
#include<d3d12.h>
#include<wrl.h>

class Model {

	//サブクラス
	//頂点データ構造体
	struct Vertex {
		Vector3 pos;	//xyz座標
		Vector3 normal;	//法線ベクトル
		Vector2 uv;		//uv座標
	};

	//定数バッファ用構造体
	struct ConstBufferDataMaterial {
		Vector3 ambient;//アンビエント係数
		float pad1;		//パディング
		Vector3 diffuse;//ディフューズ係数
		float pad2;		//パディング
		Vector3 specular;//スペキュラー係数
		float alpha;	//アルファ
	};

	//マテリアル
	struct Material {
		std::string name;	//マテリアル名
		Vector3 ambient;	//アンビエント影響度
		Vector3 diffuse;	//ディフューズ影響度
		Vector3 specular;	//スペキュラー影響度
		float alpha;		//アルファ
		std::string textureFileName;//テクスチャファイル名
		//コンストラクタ
		Material() {
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};

	//メンバ変数
	std::vector<Vertex> vertices;		//頂点データ配列
	D3D12_VERTEX_BUFFER_VIEW vbView;	//頂点バッファビュー
	std::vector<unsigned short> indices;//インデックスデータ配列
	D3D12_INDEX_BUFFER_VIEW ibView;		//インデックスバッファビュー
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;	//頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff;	//インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;	//インデックスバッファ

	uint32_t textureIndex = 0;	//テクスチャ番号
	Material material;			//マテリアル

public:
	static ID3D12Device* device;

	//メンバ関数
	static Model* CreateModel(const std::string& filename = "NULL");

	static void SetDevice(ID3D12Device* dev) { device = dev; }

	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex);


	//内部処理用の非公開メンバ関数
private:

	//モデル生成
	void Create(const std::string& modelname);
	void LoadTexture(const std::string& directoryPath, const std::string& filename);
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);
	void CreateBuffers();
};
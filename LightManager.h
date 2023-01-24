#pragma once
#include "DirectX.h"
#include "PointLight.h"
#include "Light.h"

class LightManager
{
private://エイリアス
//Microsoft::WRL::を省略
	template<class T> using Comptr = Microsoft::WRL::ComPtr<T>;
	//DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

private://定数
//点光源の数
	static const int PointLightNum = 3;
	//平行光源
	static const int DirLightNum = 3;

private://静的メンバ変数
	static ID3D12Device* device_;

private://メンバ変数
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	//ダーティフラグ
	bool dirty = false;
	// 環境光の色
	XMFLOAT3 ambientColor = { 1,1,1 };
	//点光源の配列
	PointLight pointLights[PointLightNum];
	// 平行光源の配列
	Light dirLights[DirLightNum];

public://サブクラス
	//定数バッファ用データ構造体(平行光源、点光源の配列をまとめて送る)
	struct ConstBufferData
	{
		// 環境光の色
		XMFLOAT3 ambientColor;
		float pad1;

		// 平行光源の配列
		Light::ConstBufferData dirLights[DirLightNum];
		//点光源用
		PointLight::ConstBufferData pointLights[PointLightNum];
	};


private:
	/// <summary>
/// 定数バッファ転送
/// </summary>
	void TransferConstBuffer();

public://静的メンバ関数
	static void StaticInitialize();

public:
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画（描画はせず、定数バッファビューのセットのみ行う）
	/// </summary>
	/// <param name="rootParamaterIndex"></param>
	void Draw(UINT rootParamaterIndex);

	/// <summary>
	/// 標準のライト設定
	/// </summary>
	void DefaultLightSetting();

	/// <summary>
	/// 環境光のライト色をセット
	/// </summary>
	/// <param name="color">ライト色</param>
	void SetAmbientColor(const XMFLOAT3& color);

	/// <summary>
	/// インスタンス生成
	/// </summary>
	static LightManager* Create();

	/// <summary>
		/// 平行光源の有効フラグをセット
		/// </summary>
		/// <param name="index">ライト番号</param>
		/// <param name="active">有効フラグ</param>
	void SetDirLightActive(int index, bool active);

	/// <summary>
	/// 平行光源のライト方向をセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="lightdir">ライト方向</param>
	void SetDirLightDir(int index, const XMVECTOR& lightdir);

	/// <summary>
	/// 平行光源のライト色をセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="lightcolor">ライト色</param>
	void SetDirLightColor(int index, const XMFLOAT3& lightcolor);

	void SetPointLightActive(int index, bool active);
	void SetPointLightPos(int index, const XMFLOAT3& pos);
	void SetPointLightColor(int index, const XMFLOAT3& color);
	void SetPointLightAtten(int index, const XMFLOAT3& atten);
};


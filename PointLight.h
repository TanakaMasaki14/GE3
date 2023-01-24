#pragma once
#include<DirectXMath.h>
#include <wrl.h>
using namespace Microsoft::WRL;

/// <summary>
/// 点光源
/// </summary>
class PointLight
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

public://サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferData
	{
		XMFLOAT3 lightpos;
		float pad1;
		XMFLOAT3 lightcolor;
		float pad2;
		XMFLOAT3 lightatten;
		unsigned int active;
	};

private://メンバ変数
	//ライト座標（ワールド座標系）
	XMFLOAT3 lightpos = { 0,0,0 };
	//ライト色
	XMFLOAT3 lightcolor = { 1.0f,1.0f,1.0f };
	//ライト距離減衰係数
	XMFLOAT3 lightatten = { 1.0f,1.0f,1.0f };
	//有効フラグ
	bool active = false;



public://メンバ関数
	inline void SetLightPos(const XMFLOAT3& lightpos) { this->lightpos = lightpos; }
	inline const XMFLOAT3& GetLightPos() { return lightpos; }
	inline void SetLightColor(const XMFLOAT3& lightcolor) { this->lightcolor = lightcolor; }
	inline const XMFLOAT3& GetLightColor() { return lightcolor; }
	inline void SetLightAtten(const XMFLOAT3& lightatten) { this->lightatten = lightatten; }
	inline const XMFLOAT3& GetLightAtten() { return lightatten; }
	inline void SetActive(bool active) { this->active = active; }
	inline bool GetActive() { return active; }
};


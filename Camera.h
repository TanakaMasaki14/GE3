#pragma once
#include"DirectX.h"
#include<DirectXMath.h>
#include<d3d12.h>
#include<wrl.h>

class Camera
{
public:
	static ID3D12Device* device;

	static void StaticInitialize(ID3D12Device* dev);

	struct ConstBufferCamera {
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	ConstBufferCamera* constMap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;

	DirectX::XMMATRIX matView;
	DirectX::XMMATRIX matProjection;
	DirectX::XMFLOAT3 eye;
	DirectX::XMFLOAT3 target;
	DirectX::XMFLOAT3 up;

public:


	void Initialize(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

	void UpdateMatrix();

	DirectX::XMMATRIX GetViewProjection()const { return matView * matProjection; }
};


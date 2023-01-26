#include "Gridline.h"
using namespace Microsoft::WRL;

void Gridline::Initialize(ComPtr<ID3D12Device> device, int lineNum, D3D12_CPU_DESCRIPTOR_HANDLE& srvHandle)
{
	HRESULT result;

	//引数からラインの長さを取得
	Vector3 length = end - start;
	Vector3 center = (start + end) / 2.0f;
	Vector3 lineSpace = length / (float)lineNum;

	//引数から頂点データをメンバ変数に書き込む
	for (int i = 0; i < lineNum; i++) {
		Vertex ver{};
		ver.pos = { start.x + (lineSpace.x * i),center.y,start.z };
		vertices.push_back(ver);
		ver.pos = { start.x + (lineSpace.x * i) ,center.y,start.z + length.z };
		vertices.push_back(ver);
		ver.pos = { start.x ,center.y,start.z + (lineSpace.z * i) };
		vertices.push_back(ver);
		ver.pos = { start.x + length.x,center.y,start.z + (lineSpace.z * i) };
		vertices.push_back(ver);
	}

	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * vertices.size());

	//リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // 頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{}; // ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用

	// 頂点バッファの生成
	result = device->CreateCommittedResource(
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	// 全頂点に対して
	for (int i = 0; i < vertices.size(); i++) {
		vertMap[i] = vertices[i]; // 座標をコピー
	}
	// 繋がりを解除
	vertBuff->Unmap(0, nullptr);

	//GPU仮想アドレス
	vbview.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbview.SizeInBytes = sizeVB;
	//頂点一つ分のデータのサイズ
	vbview.StrideInBytes = sizeof(vertices[0]);

	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{  };
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;	//256バイトアラインメント
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
		IID_PPV_ARGS(&constBuffTransform));
	assert(SUCCEEDED(result));
	//定数バッファのマッピング
	result = constBuffTransform->Map(0, nullptr, (void**)&constMapTransform);//マッピング
	assert(SUCCEEDED(result));
}

void Gridline::Draw(ID3D12GraphicsCommandList* commandList, ID3D12DescriptorHeap* srvheaps)
{
	//プリミティブトポロジーを線形リストにセット
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	//頂点バッファビューの設定コマンド
	commandList->IASetVertexBuffers(0, 1, &vbview);

	//0番定数バッファビュー(CBV)の設定コマンド
	commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());

	//描画コマンド
	commandList->DrawInstanced(vertices.size(), 1, 0, 0);

}

void Gridline::Update(DirectX::XMMATRIX& matView, DirectX::XMMATRIX& matProjection)
{
	constMapTransform->mat = matView * matProjection;
}

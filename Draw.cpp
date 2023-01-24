#include "Draw.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <d3dx12.h>

//図形のクラス
Primitive primitive;


//テクスチャ

// 頂点レイアウト
D3D12_INPUT_ELEMENT_DESC inputLayout[3] = {
{//xyz座標
 "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
 D3D12_APPEND_ALIGNED_ELEMENT,
 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
}, // (1行で書いたほうが見やすい)

	{//法線ベクトル
 "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
 D3D12_APPEND_ALIGNED_ELEMENT,
 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
}, // (1行で書いたほうが見やすい)

{//uv座標
 "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
 D3D12_APPEND_ALIGNED_ELEMENT,
 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
} // (1行で書いたほうが見やすい)
};

//sprite
D3D12_INPUT_ELEMENT_DESC inputLayoutSprite[2] = {
	{//xyz座標
	 "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}, // (1行で書いたほうが見やすい)

	{//uv座標
	 "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	 D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	} // (1行で書いたほうが見やすい)
};
PipeLineSet pipelineSet;
//al4_02_02
PipeLineSet pipelineSetM;

//ルートパラメータの設定
D3D12_ROOT_PARAMETER rootParams[5] = {};

// パイプランステートの生成
ComPtr < ID3D12PipelineState> pipelineState[3] = { nullptr };
// ルートシグネチャ
ComPtr<ID3D12RootSignature> rootSignature;
// グラフィックスパイプライン設定
D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト
ID3DBlob* errorBlob = nullptr; // エラーオブジェクト

// 2.描画先の変更
	// レンダーターゲットビューのハンドルを取得
D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;

//static
LightManager* Draw::lightManager = nullptr;


void DrawInitialize()
{
	//テクスチャ用のデスクリプタヒープ初期化
	TextureManager::GetInstance().InitializeDescriptorHeap();

	//図形クラスの
	primitive.Initialize();

	//ルートパラメータの設定
	//定数バッファ0番（色）
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0;//定数バッファ番号(b0)
	rootParams[0].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//テクスチャレジスタ0番（テクスチャ）
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//デスクリプタ
	rootParams[1].DescriptorTable.pDescriptorRanges = &TextureManager::GetInstance().descriptorRange;//デスクリプタレンジ
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;//〃数
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//定数バッファ1番(行列)
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams[2].Descriptor.ShaderRegister = 1;//定数バッファ番号(b1)
	rootParams[2].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//定数バッファ2番（マテリアル）
	rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams[3].Descriptor.ShaderRegister = 2;//定数バッファ番号(b2)
	rootParams[3].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える
	//定数バッファ3番(ライト用)
	rootParams[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//定数バッファビュー
	rootParams[4].Descriptor.ShaderRegister = 3;//定数バッファ番号(b3)
	rootParams[4].Descriptor.RegisterSpace = 0;//デフォルト値
	rootParams[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//全てのシェーダから見える


	// パイプランステートの生成
	PipeLineState(D3D12_FILL_MODE_SOLID, pipelineState->GetAddressOf(), rootSignature.GetAddressOf(), vsBlob, psBlob);

	PipeLineState(D3D12_FILL_MODE_WIREFRAME, (pipelineState + 1)->GetAddressOf(), rootSignature.GetAddressOf(), vsBlob, psBlob);

	//line
	PipeLineState(D3D12_FILL_MODE_WIREFRAME, (pipelineState + 2)->GetAddressOf(), rootSignature.GetAddressOf(), vsBlob, psBlob, LINE);

	//sprite用
	PipeLineState(D3D12_FILL_MODE_SOLID, pipelineSet.pipelineState.GetAddressOf(),
		pipelineSet.rootSignature.GetAddressOf(), pipelineSet.vsBlob,
		pipelineSet.psBlob, SPRITE);

	//model用
	PipeLineState(D3D12_FILL_MODE_SOLID, pipelineSetM.pipelineState.GetAddressOf(),
		pipelineSetM.rootSignature.GetAddressOf(), pipelineSetM.vsBlob,
		pipelineSetM.psBlob, MODEL);
}

Draw::Draw()
{
	//行列
	cbt.Initialize(Directx::GetInstance());

	//スプライトクラスの
	sprite.Initialize();

	//03_02
	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	ResourceProperties(cbResourceDesc,
		((UINT)sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff/*256バイトアライメント*/);
	//定数バッファの生成
	BuffProperties(cbHeapProp, cbResourceDesc, &constBuffMaterial);
	//定数バッファのマッピング
	Directx::GetInstance().result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);//マッピング
	assert(SUCCEEDED(Directx::GetInstance().result));
}



void Draw::Update(const int& indexNum, const int& pipelineNum, const UINT64 textureHandle, const ConstBuffTransform& constBuffTransform,
	Model* model, const bool& primitiveMode)
{
	worldMat->SetWorld();

	//変換行列をGPUに送信
	if (worldMat->parent != nullptr && indexNum != SPRITE)//親がいる場合
	{
		worldMat->matWorld *= worldMat->parent->matWorld;
		XMMATRIX matW;
		matW = { (float)worldMat->matWorld.m[0][0],(float)worldMat->matWorld.m[0][1],(float)worldMat->matWorld.m[0][2],(float)worldMat->matWorld.m[0][3],
				 (float)worldMat->matWorld.m[1][0],(float)worldMat->matWorld.m[1][1],(float)worldMat->matWorld.m[1][2],(float)worldMat->matWorld.m[1][3],
				 (float)worldMat->matWorld.m[2][0],(float)worldMat->matWorld.m[2][1],(float)worldMat->matWorld.m[2][2],(float)worldMat->matWorld.m[2][3],
				 (float)worldMat->matWorld.m[3][0],(float)worldMat->matWorld.m[3][1],(float)worldMat->matWorld.m[3][2],(float)worldMat->matWorld.m[3][3] };

		cbt.constMapTransform->world = matW;
		cbt.constMapTransform->viewproj = view->matView * projection->matProjection;
		XMFLOAT3 cPos = { view->eye.x,view->eye.y,view->eye.z };
		cbt.constMapTransform->cameraPos = cPos;

	}
	else if (indexNum != SPRITE)//親がいない場合
	{
		XMMATRIX matW;
		matW = { (float)worldMat->matWorld.m[0][0],(float)worldMat->matWorld.m[0][1],(float)worldMat->matWorld.m[0][2],(float)worldMat->matWorld.m[0][3],
				 (float)worldMat->matWorld.m[1][0],(float)worldMat->matWorld.m[1][1],(float)worldMat->matWorld.m[1][2],(float)worldMat->matWorld.m[1][3],
				 (float)worldMat->matWorld.m[2][0],(float)worldMat->matWorld.m[2][1],(float)worldMat->matWorld.m[2][2],(float)worldMat->matWorld.m[2][3],
				 (float)worldMat->matWorld.m[3][0],(float)worldMat->matWorld.m[3][1],(float)worldMat->matWorld.m[3][2],(float)worldMat->matWorld.m[3][3] };

		cbt.constMapTransform->world = matW;
		cbt.constMapTransform->viewproj = view->matView * projection->matProjection;
		XMFLOAT3 cPos = { view->eye.x,view->eye.y,view->eye.z };
		cbt.constMapTransform->cameraPos = cPos;
	}


	if (indexNum == TRIANGLE)
	{
		// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
		Vertex* vertMap = nullptr;
		Directx::GetInstance().result = primitive.vertBuffTriangle->Map(0, nullptr, (void**)&vertMap);
		assert(SUCCEEDED(Directx::GetInstance().result));
		// 全頂点に対して
		for (int i = 0; i < _countof(primitive.verticesTriangle); i++) {

			vertMap[i] = primitive.verticesTriangle[i]; // 座標をコピー

		}
		// 繋がりを解除
		primitive.vertBuffTriangle->Unmap(0, nullptr);

		// パイプラインステートとルートシグネチャの設定コマンド
		Directx::GetInstance().GetCommandList()->SetPipelineState(pipelineState[pipelineNum].Get());

		Directx::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

		Directx::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角リスト

		Directx::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &primitive.vbTriangleView);

		Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

		lightManager->Draw(4);

		Directx::GetInstance().GetCommandList()->IASetIndexBuffer(&primitive.ibViewTriangle);

		//04_02
		{
			//SRVヒープの設定コマンド
			Directx::GetInstance().GetCommandList()->SetDescriptorHeaps(1, TextureManager::GetInstance().srvHeap.GetAddressOf());
			//SRVヒープの先頭ハンドルを取得
			D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
			srvGpuHandle.ptr = textureHandle;
			//(インスタンスで読み込んだテクスチャ用のSRVを指定)
			Directx::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
		}
		//定数バッファビュー(CBV)の設定コマンド
		Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform.constBuffTransform->GetGPUVirtualAddress());

		Directx::GetInstance().GetCommandList()->DrawIndexedInstanced(_countof(indicesTriangle), 1, 0, 0, 0); // 全ての頂点を使って描画

	}
	else if (indexNum == BOX)
	{
		// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
		Vertex* vertMap = nullptr;
		Directx::GetInstance().result = primitive.vertBuffBox->Map(0, nullptr, (void**)&vertMap);
		assert(SUCCEEDED(Directx::GetInstance().result));
		// 全頂点に対して
		for (int i = 0; i < _countof(primitive.verticesBox); i++) {

			vertMap[i] = primitive.verticesBox[i]; // 座標をコピー

		}
		// 繋がりを解除
		primitive.vertBuffBox->Unmap(0, nullptr);

		// パイプラインステートとルートシグネチャの設定コマンド
		Directx::GetInstance().GetCommandList()->SetPipelineState(pipelineState[pipelineNum].Get());

		Directx::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

		Directx::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角リスト

		Directx::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &primitive.vbBoxView);

		Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

		lightManager->Draw(4);

		Directx::GetInstance().GetCommandList()->IASetIndexBuffer(&primitive.ibViewBox);

		//04_02
		{
			//SRVヒープの設定コマンド
			Directx::GetInstance().GetCommandList()->SetDescriptorHeaps(1, TextureManager::GetInstance().srvHeap.GetAddressOf());
			//SRVヒープの先頭ハンドルを取得
			D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
			srvGpuHandle.ptr = textureHandle;
			//(インスタンスで読み込んだテクスチャ用のSRVを指定)
			Directx::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
		}
		//定数バッファビュー(CBV)の設定コマンド
		Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform.constBuffTransform->GetGPUVirtualAddress());

		Directx::GetInstance().GetCommandList()->DrawIndexedInstanced(_countof(indicesBox), 1, 0, 0, 0); // 全ての頂点を使って描画
	}
	else if (indexNum == CUBE)
	{
		// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
		Vertex* vertMap = nullptr;
		Directx::GetInstance().result = primitive.vertBuffCube->Map(0, nullptr, (void**)&vertMap);
		assert(SUCCEEDED(Directx::GetInstance().result));
		// 全頂点に対して
		for (int i = 0; i < _countof(primitive.verticesCube); i++) {

			vertMap[i] = primitive.verticesCube[i]; // 座標をコピー

		}
		// 繋がりを解除
		primitive.vertBuffCube->Unmap(0, nullptr);

		// パイプラインステートとルートシグネチャの設定コマンド
		Directx::GetInstance().GetCommandList()->SetPipelineState(pipelineState[pipelineNum].Get());

		Directx::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

		Directx::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角リスト

		Directx::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &primitive.vbCubeView);

		Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

		lightManager->Draw(4);

		Directx::GetInstance().GetCommandList()->IASetIndexBuffer(&primitive.ibViewCube);

		//04_02
		{
			//SRVヒープの設定コマンド
			Directx::GetInstance().GetCommandList()->SetDescriptorHeaps(1, TextureManager::GetInstance().srvHeap.GetAddressOf());
			//SRVヒープの先頭ハンドルを取得
			D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
			srvGpuHandle.ptr = textureHandle;
			//(インスタンスで読み込んだテクスチャ用のSRVを指定)
			Directx::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
		}
		//定数バッファビュー(CBV)の設定コマンド
		Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform.constBuffTransform->GetGPUVirtualAddress());

		Directx::GetInstance().GetCommandList()->DrawIndexedInstanced(_countof(indicesCube), 1, 0, 0, 0); // 全ての頂点を使って描画
	}
	else if (indexNum == LINE)
	{
		// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
		Vertex* vertMap = nullptr;
		Directx::GetInstance().result = primitive.vertBuffLine->Map(0, nullptr, (void**)&vertMap);
		assert(SUCCEEDED(Directx::GetInstance().result));
		// 全頂点に対して
		for (int i = 0; i < _countof(primitive.verticesLine); i++) {

			vertMap[i] = primitive.verticesLine[i]; // 座標をコピー

		}
		// 繋がりを解除
		primitive.vertBuffLine->Unmap(0, nullptr);

		//パイプライン
		Directx::GetInstance().GetCommandList()->SetPipelineState(pipelineState[2].Get());

		Directx::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

		Directx::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST); // 線

		Directx::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &primitive.vbLineView);

		Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

		lightManager->Draw(4);

		Directx::GetInstance().GetCommandList()->IASetIndexBuffer(&primitive.ibViewLine);

		//04_02
		{
			//SRVヒープの設定コマンド
			Directx::GetInstance().GetCommandList()->SetDescriptorHeaps(1, TextureManager::GetInstance().srvHeap.GetAddressOf());
			//SRVヒープの先頭ハンドルを取得
			D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
			srvGpuHandle.ptr = textureHandle;
			//(インスタンスで読み込んだテクスチャ用のSRVを指定)
			Directx::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
		}
		//定数バッファビュー(CBV)の設定コマンド
		Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform.constBuffTransform->GetGPUVirtualAddress());

		Directx::GetInstance().GetCommandList()->DrawIndexedInstanced(_countof(indicesLine), 1, 0, 0, 0); // 全ての頂点を使って描画
	}
	else if (indexNum == CIRCLE)
	{
		// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
		Vertex* vertMap = nullptr;
		Directx::GetInstance().result = primitive.vertBuffCircle->Map(0, nullptr, (void**)&vertMap);
		assert(SUCCEEDED(Directx::GetInstance().result));
		// 全頂点に対して
		for (int i = 0; i < _countof(primitive.verticesCircle); i++) {

			vertMap[i] = primitive.verticesCircle[i]; // 座標をコピー

		}
		// 繋がりを解除
		primitive.vertBuffCircle->Unmap(0, nullptr);

		// パイプラインステートとルートシグネチャの設定コマンド
		Directx::GetInstance().GetCommandList()->SetPipelineState(pipelineState[pipelineNum].Get());

		Directx::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

		Directx::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角リスト

		Directx::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &primitive.vbCircleView);

		Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

		lightManager->Draw(4);

		Directx::GetInstance().GetCommandList()->IASetIndexBuffer(&primitive.ibViewCircle);

		//04_02
		{
			//SRVヒープの設定コマンド
			Directx::GetInstance().GetCommandList()->SetDescriptorHeaps(1, TextureManager::GetInstance().srvHeap.GetAddressOf());
			//SRVヒープの先頭ハンドルを取得
			D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
			srvGpuHandle.ptr = textureHandle;
			//(インスタンスで読み込んだテクスチャ用のSRVを指定)
			Directx::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
		}
		//定数バッファビュー(CBV)の設定コマンド
		Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform.constBuffTransform->GetGPUVirtualAddress());

		Directx::GetInstance().GetCommandList()->DrawIndexedInstanced(_countof(indicesCircle), 1, 0, 0, 0); // 全ての頂点を使って描画
	}
	else if (indexNum == SPHERE)
	{

		// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得
		Vertex* vertMap = nullptr;
		Directx::GetInstance().result = primitive.vertBuffSphere->Map(0, nullptr, (void**)&vertMap);
		assert(SUCCEEDED(Directx::GetInstance().result));
		// 全頂点に対して
		for (int i = 0; i < _countof(primitive.verticesSphere); i++) {

			vertMap[i] = primitive.verticesSphere[i]; // 座標をコピー

		}
		// 繋がりを解除
		primitive.vertBuffSphere->Unmap(0, nullptr);

		// パイプラインステートとルートシグネチャの設定コマンド
		Directx::GetInstance().GetCommandList()->SetPipelineState(pipelineState[pipelineNum].Get());

		Directx::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

		Directx::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角ストリップ
		// 頂点バッファビューの設定コマンド
		Directx::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &primitive.vbViewSphere);

		Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

		lightManager->Draw(4);

		Directx::GetInstance().GetCommandList()->IASetIndexBuffer(&primitive.ibViewSphere);

		//04_02
		{
			//SRVヒープの設定コマンド
			Directx::GetInstance().GetCommandList()->SetDescriptorHeaps(1, TextureManager::GetInstance().srvHeap.GetAddressOf());
			//SRVヒープの先頭ハンドルを取得
			D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
			srvGpuHandle.ptr = textureHandle;
			//(インスタンスで読み込んだテクスチャ用のSRVを指定)
			Directx::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
		}
		//定数バッファビュー(CBV)の設定コマンド
		Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform.constBuffTransform->GetGPUVirtualAddress());

		Directx::GetInstance().GetCommandList()->DrawIndexedInstanced(_countof(primitive.indicesSphere), 1, 0, 0, 0); // 全ての頂点を使って描画
	}
	else if (indexNum == SPRITE)
	{
		SpriteCommonBeginDraw(&pipelineSet);

		Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());


		//04_02
		{
			//SRVヒープの設定コマンド
			Directx::GetInstance().GetCommandList()->SetDescriptorHeaps(1, TextureManager::GetInstance().srvHeap.GetAddressOf());
			//SRVヒープの先頭ハンドルを取得
			D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
			srvGpuHandle.ptr = textureHandle;
			//(インスタンスで読み込んだテクスチャ用のSRVを指定)
			Directx::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
		}
		//定数バッファビュー(CBV)の設定コマンド
		Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform.constBuffTransform->GetGPUVirtualAddress());

		sprite.SpriteDraw();
	}
	else if (indexNum == MODEL)
	{
		// パイプラインステートとルートシグネチャの設定コマンド
		Directx::GetInstance().GetCommandList()->SetPipelineState(pipelineSetM.pipelineState.Get());

		Directx::GetInstance().GetCommandList()->SetGraphicsRootSignature(pipelineSetM.rootSignature.Get());

		Directx::GetInstance().GetCommandList()->IASetVertexBuffers(0, 1, &model->vbViewM);
		Directx::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

		Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(3, model->constBuffMaterial2->GetGPUVirtualAddress());

		lightManager->Draw(4);

		//04_02
		{
			//SRVヒープの設定コマンド
			Directx::GetInstance().GetCommandList()->SetDescriptorHeaps(1, TextureManager::GetInstance().srvHeap.GetAddressOf());
			//SRVヒープの先頭ハンドルを取得
			D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
			srvGpuHandle.ptr = textureHandle;
			//(インスタンスで読み込んだテクスチャ用のSRVを指定)
			Directx::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
		}
		//定数バッファビュー(CBV)の設定コマンド
		Directx::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform.constBuffTransform->GetGPUVirtualAddress());

		Directx::GetInstance().GetCommandList()->IASetIndexBuffer(&model->ibViewM);

		Directx::GetInstance().GetCommandList()->DrawIndexedInstanced((UINT)model->indicesM.size(), 1, 0, 0, 0); // 全ての頂点を使って描画
	}
}

void Draw::DrawTriangle(/*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3,*/
	WorldMat* world, ViewMat* view, ProjectionMat* projection, XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	//constBuffTransfer({ 0.01f,0.05f,0.1f,0 });
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	Update(TRIANGLE, pipelineNum, textureHandle, cbt);
}

void Draw::DrawBox(WorldMat* world, ViewMat* view, ProjectionMat* projection,/*XMFLOAT3& pos1, XMFLOAT3& pos2, XMFLOAT3& pos3, XMFLOAT3& pos4, */
	XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	//constBuffTransfer({ 0.01f,0.05f,0.1f,0 });
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	Update(BOX, pipelineNum, textureHandle, cbt);
}

void Draw::DrawBoxSprite(const Vec3& pos, const float& scale,
	XMFLOAT4 color, const UINT64 textureHandle, const Vec2& ancorUV, const bool& isReverseX, const bool& isReverseY,
	float rotation, const int& pipelineNum)
{
	sprite.Update(pos, scale, color, textureHandle, ancorUV, isReverseX, isReverseY, rotation, &cbt, constMapMaterial);

	Update(SPRITE, pipelineNum, textureHandle, cbt);
}

void Draw::DrawClippingBoxSprite(const Vec3& leftTop, const float& scale, const XMFLOAT2& UVleftTop, const XMFLOAT2& UVlength,
	XMFLOAT4 color, const UINT64 textureHandle, bool isPosLeftTop, const bool& isReverseX, const bool& isReverseY,
	float rotation, const int& pipelineNum)
{
	sprite.UpdateClipping(leftTop, scale, UVleftTop, UVlength, color, textureHandle,
		isPosLeftTop, isReverseX, isReverseY, rotation, &cbt, constMapMaterial);

	Update(SPRITE, pipelineNum, textureHandle, cbt);
}

void Draw::DrawCube3D(WorldMat* world, ViewMat* view, ProjectionMat* projection, XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	Update(CUBE, pipelineNum, textureHandle, cbt);
}

void Draw::DrawLine(/*const Vec3& pos1, const Vec3& pos2,*/ WorldMat* world, ViewMat* view, ProjectionMat* projection, const XMFLOAT4& color,
	const UINT64 textureHandle, const int& pipelineNum)
{
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	Update(LINE, pipelineNum, textureHandle, cbt, nullptr, false);
}

void Draw::DrawCircle(float radius, WorldMat* world, ViewMat* view, ProjectionMat* projection,
	XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	this->worldMat = world;
	this->worldMat->scale = { radius,radius,radius };
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	Update(CIRCLE, pipelineNum, textureHandle, cbt);
}

void Draw::DrawSphere(WorldMat* world, ViewMat* view, ProjectionMat* projection,
	XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	Update(SPHERE, pipelineNum, textureHandle, cbt);
}

void Draw::DrawModel(WorldMat* world, ViewMat* view, ProjectionMat* projection,
	Model* model, XMFLOAT4 color, const UINT64 textureHandle, const int& pipelineNum)
{
	this->worldMat = world;
	this->view = view;
	this->projection = projection;

	constMapMaterial->color = color;

	if (textureHandle == NULL)
		Update(MODEL, pipelineNum, model->material.textureHandle, cbt, model);
	else
		Update(MODEL, pipelineNum, textureHandle, cbt, model);
}

void PipeLineState(const D3D12_FILL_MODE& fillMode, ID3D12PipelineState** pipelineState, ID3D12RootSignature** rootSig,
	ID3DBlob* vsBlob, ID3DBlob* psBlob, const int& indexNum)
{
	if (indexNum == SPRITE)
	{
		// 頂点シェーダの読み込みとコンパイル
		Directx::GetInstance().result = D3DCompileFromFile(
			L"Resources/shaders/SpriteVS.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&vsBlob, &errorBlob);

		// エラーなら
		Error(FAILED(Directx::GetInstance().result));

		// ピクセルシェーダの読み込みとコンパイル
		Directx::GetInstance().result = D3DCompileFromFile(
			L"Resources/shaders/SpritePS.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&psBlob, &errorBlob);

		// エラーなら
		Error(FAILED(Directx::GetInstance().result));
	}
	else if (indexNum == MODEL)
	{
		// 頂点シェーダの読み込みとコンパイル
		Directx::GetInstance().result = D3DCompileFromFile(
			L"Resources/shaders/OBJVertexShader.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&vsBlob, &errorBlob);

		// エラーなら
		Error(FAILED(Directx::GetInstance().result));

		// ピクセルシェーダの読み込みとコンパイル
		Directx::GetInstance().result = D3DCompileFromFile(
			L"Resources/shaders/OBJPixelShader.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&psBlob, &errorBlob);

		// エラーなら
		Error(FAILED(Directx::GetInstance().result));
	}
	else
	{
		// 頂点シェーダの読み込みとコンパイル
		Directx::GetInstance().result = D3DCompileFromFile(
			L"Resources/shaders/BasicVS.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&vsBlob, &errorBlob);

		// エラーなら
		Error(FAILED(Directx::GetInstance().result));

		// ピクセルシェーダの読み込みとコンパイル
		Directx::GetInstance().result = D3DCompileFromFile(
			L"Resources/shaders/BasicPS.hlsl", // シェーダファイル名
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
			"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
			0,
			&psBlob, &errorBlob);

		// エラーなら
		Error(FAILED(Directx::GetInstance().result));
	}

	// シェーダーの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	// サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

	// ラスタライザの設定
	if (indexNum == SPRITE)
	{
		pipelineDesc.RasterizerState = D3D12_RASTERIZER_DESC();
		pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	}
	else
		pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // 背面カリング

	pipelineDesc.RasterizerState.FillMode = fillMode; // ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

	Blend(D3D12_BLEND_OP_ADD, false, true);

	// 頂点レイアウトの設定
	if (indexNum == SPRITE)
	{
		pipelineDesc.InputLayout.pInputElementDescs = inputLayoutSprite;
		pipelineDesc.InputLayout.NumElements = _countof(inputLayoutSprite);
	}
	else
	{
		pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
		pipelineDesc.InputLayout.NumElements = _countof(inputLayout);
	}

	// 図形の形状設定
	if (indexNum == LINE)
		pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	else
		pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// その他の設定
	pipelineDesc.NumRenderTargets = 1; // 描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	//04_02
	//テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //横繰り返し（タイリング）
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //縦繰り返し（タイリング）
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  //奥行き繰り返し（タイリング）
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;    //全てリニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;                  //ミップマップ最大値
	samplerDesc.MinLOD = 0.0f;                               //ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダーからのみ使用可能

	// ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;//ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams);//ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;
	// ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	Directx::GetInstance().result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(Directx::GetInstance().result));
	Directx::GetInstance().result = Directx::GetInstance().GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(rootSig));
	assert(SUCCEEDED(Directx::GetInstance().result));
	// パイプラインにルートシグネチャをセット
	pipelineDesc.pRootSignature = *rootSig;

	//06_01
	//デプスステンシルステート
	pipelineDesc.DepthStencilState = D3D12_DEPTH_STENCIL_DESC();
	pipelineDesc.DepthStencilState.DepthEnable = true;//深度テストを行う
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込み許可
	if (indexNum == SPRITE)
		pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//小さければ合格
	else
		pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//小さければ合格
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

	Directx::GetInstance().result = Directx::GetInstance().GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(pipelineState));
	//assert(SUCCEEDED(Directx::GetInstance().result));
}

void Blend(const D3D12_BLEND_OP& blendMode, const bool& Inversion, const bool& Translucent)
{
	//共通設定
	D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = pipelineDesc.BlendState.RenderTarget[0];
	blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;// RBGA全てのチャンネルを描画
	blendDesc.BlendEnable = true;//ブレンドを有効
	blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	//合成ごと
	blendDesc.BlendOp = blendMode;
	if (Inversion)//反転
	{
		blendDesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f-デストカラーの値
		blendDesc.DestBlend = D3D12_BLEND_ZERO;//使わない
	}
	else if (Translucent)//半透明
	{
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;//1.0f-デストカラーの値
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値
	}
	else
	{
		blendDesc.SrcBlend = D3D12_BLEND_ONE;//ソースの値を100%使う
		blendDesc.DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う
	}
}

void Draw::constBuffTransfer(const XMFLOAT4& plusRGBA)
{
	if (constMapMaterial->color.x <= 1.0f && constMapMaterial->color.x >= 0.0f)
		constMapMaterial->color.x += plusRGBA.x;
	if (constMapMaterial->color.y <= 1.0f && constMapMaterial->color.y >= 0.0f)
		constMapMaterial->color.y += plusRGBA.y;
	if (constMapMaterial->color.z <= 1.0f && constMapMaterial->color.z >= 0.0f)
		constMapMaterial->color.z += plusRGBA.z;
	if (constMapMaterial->color.w <= 1.0f && constMapMaterial->color.w >= 0.0f)
		constMapMaterial->color.w += plusRGBA.w;
}



void SetNormDigitalMat(XMMATRIX& mat)
{
	mat.r[0].m128_f32[0] = 2.0f / WindowsApp::GetInstance().window_width;
	mat.r[1].m128_f32[1] = -2.0f / WindowsApp::GetInstance().window_height;
	mat.r[3].m128_f32[0] = -1.0f;
	mat.r[3].m128_f32[1] = 1.0f;
}

void Error(const bool& filed)
{
	if (filed)
	{
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
}

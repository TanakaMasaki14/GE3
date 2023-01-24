#pragma once
#include"Model.h"

static const int PAPA = 66 * 3 + 6;


/// <summary>
/// 図形用の情報を持ったクラス（drawで使うので一個のみ生成）
/// </summary>
class Primitive
{
private:

public:
	//頂点バッファの生成
	ComPtr < ID3D12Resource> vertBuffTriangle = nullptr;
	ComPtr < ID3D12Resource> vertBuffBox = nullptr;
	ComPtr < ID3D12Resource> vertBuffCircle = nullptr;
	ComPtr < ID3D12Resource> vertBuffCube = nullptr;
	ComPtr < ID3D12Resource> vertBuffLine = nullptr;
	ComPtr<ID3D12Resource> vertBuffSphere = nullptr;

	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbTriangleView{};
	D3D12_VERTEX_BUFFER_VIEW vbBoxView{};
	D3D12_VERTEX_BUFFER_VIEW vbCircleView{};
	D3D12_VERTEX_BUFFER_VIEW vbCubeView{};
	D3D12_VERTEX_BUFFER_VIEW vbLineView{};
	// 頂点バッファビューの作成
	D3D12_VERTEX_BUFFER_VIEW vbViewSphere{};

	//04_01
		//頂点データ構造体

	//頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{};

	//インデックスバッファビューの作成
	D3D12_INDEX_BUFFER_VIEW ibViewTriangle{};
	D3D12_INDEX_BUFFER_VIEW ibViewBox{};
	D3D12_INDEX_BUFFER_VIEW ibViewCircle{};
	D3D12_INDEX_BUFFER_VIEW ibViewLine{};
	D3D12_INDEX_BUFFER_VIEW ibViewCube{};
	D3D12_INDEX_BUFFER_VIEW ibViewSphere{};

	//いろんな図形用
	Vertex verticesTriangle[3] = {
		//手前
		{{-5.0f,-5.0f,-5.0f},{},{0.0f,1.0f}},//左下
		{{-5.0f,5.0f, -5.0f},{},{0.0f,0.0f}},//左上
		{{5.0f,-5.0f, -5.0f},{},{1.0f,1.0f}},//右下
	};
	Vertex verticesBox[4] = {
		//手前
		{{-5.0f,-5.0f,-5.0f},{},{0.0f,1.0f}},//左下
		{{-5.0f,5.0f, -5.0f},{},{0.0f,0.0f}},//左上
		{{5.0f,-5.0f, -5.0f},{},{1.0f,1.0f}},//右下
		{{5.0f,5.0f,  -5.0f},{},{1.0f,0.0f}},//右上
	};
	Vertex verticesCircle[24] = {};
	Vertex verticesCube[24] = {};
	Vertex verticesLine[2] = {};
	//球体
	Vertex verticesSphere[2 + 34 * 36];

	unsigned short indicesSphere[PAPA * 36];


public:
	void Initialize();

	void InitializeTriangle();
	void InitializeBox();
	void InitializeCircle();
	void InitializeCube();
	void InitializeLine();
	void InitializeSphere();

};


static unsigned short indicesBox[6] =
{
	0,1,2,//三角形1つ目
	2,1,3,//三角形2つ目
};
static unsigned short indicesTriangle[3] =
{
	0,1,2//三角形2つ目
};
static unsigned short indicesCube[36] =
{
	//前
	0,1,2,//三角形1つ目
	2,1,3,//三角形2つ目
	//奥
	6,5,4,//三角形1つ目
	7,5,6,//三角形2つ目
	//上
	10,9,8,//三角形1つ目
	11,9,10,//三角形2つ目
	//下
	12,13,14,//三角形1つ目
	14,13,15,//三角形2つ目
	//左
	16,17,18,//三角形1つ目
	18,17,19,//三角形2つ目
	//右
	22,21,20,//三角形1つ目
	23,21,22,//三角形2つ目
};
static unsigned short indicesCircle[] =
{
	2,1,0,
	3,2,0,
	4,3,0,
	5,4,0,
	6,5,0,
	7,6,0,
	8,7,0,
	9,8,0,
	10,9,0,
	11,10,0,
	12,11,0,
	13,12,0,
	14,13,0,
	15,14,0,
	16,15,0,
	17,16,0,
	18,17,0,
	19,18,0,
	20,19,0,
	21,20,0,
	22,21,0,
	23,22,0,
};

static unsigned short indicesLine[2] =
{
	0,1//三角形2つ目
};

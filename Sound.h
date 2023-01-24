#pragma once
#include<xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include<cassert>
#include <fstream>
#include <string>
#include <wrl.h>
using namespace Microsoft::WRL;
#include <map>
#include <vector>


class Sound final
{
public:
	//音
	static IXAudio2MasteringVoice* masterVoice;
	//音
	static ComPtr<IXAudio2> xAudio2_;


public://サブクラス
	//チャンクヘッダ
	struct ChunkHeader
	{
		char id[4];//チャンクごとのID
		int32_t size;//チャンクサイズ
	};
	//RIFFヘッダ
	struct RiffHeader
	{
		ChunkHeader chunk;//"RIFF"
		char type[4];//"WAVE"
	};
	//fmtチャンク
	struct FormatChunk
	{
		ChunkHeader chunk;//"fmt"
		WAVEFORMATEX fmt;//"波形フォーマット"
	};

	struct SoundData
	{
		//波形フォーマット
		WAVEFORMATEX wfex;
		//バッファの先頭アドレス
		BYTE* pBuffer = nullptr;
		//バッファのサイズ
		unsigned int bufferSize;
		//
		std::vector<IXAudio2SourceVoice*> pSourceVoice = { nullptr };
	};

private:
	//サウンドデータの連想配列
	static std::map < std::string, SoundData> soundDatas_;
	//サウンド格納ディレクトリ
	static std::string directoryPath_;


private:
	Sound();
	~Sound();

public://メンバ関数

	static void Initialize(const std::string& directoryPath_ = "Resources/sound/");

	/// <summary>
/// 音の読み込み（第2は基本true,読み込めない/再生されないときはfalseにする）
/// </summary>
/// <param name="filename"></param>
/// <param name="isConvert"></param>
/// <returns></returns>
	static void LoadWave(const std::string& filename, const bool& isConvert);
	/// <summary>
	/// 解放処理
	/// </summary>
	/// <param name="soudData"></param>
	/// <returns></returns>
	static void UnLoad(SoundData* soundData);


	/// <summary>
	/// 音声再生
	/// </summary>
	/// <param name="">wavファイル名</param>
	/// <param name="volume">音量</param>
	/// <param name="Loop">ループ再生</param>
	static void PlayWave(const std::string& filename, const float& volume = 5.0f, const bool& Loop = false);

	static void StopWave(const std::string& filename);


public://シングルトン
	//コピーコンストラクタを無効
	Sound(const Sound& obj) = delete;
	//代入演算子も
	Sound& operator=(const Sound& obj) = delete;

	//関数
	static Sound& GetInstance();
};







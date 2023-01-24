#include "Sound.h"

IXAudio2MasteringVoice* Sound::masterVoice;
ComPtr<IXAudio2> Sound::xAudio2_;
std::map < std::string, Sound::SoundData> Sound::soundDatas_;
std::string Sound::directoryPath_ = "Resources/sound/";

Sound::Sound()
{
}

Sound::~Sound()
{
	xAudio2_.Reset();

	//音声データ解放
	std::map < std::string, Sound::SoundData>::iterator it = soundDatas_.begin();

	for (; it != soundDatas_.end(); ++it)
	{
		//secondはペアの二つ目
		UnLoad(&it->second);
	}
	soundDatas_.clear();
}

void Sound::Initialize(const std::string& directoryPath_)
{
	Sound::directoryPath_ = directoryPath_;

	HRESULT result;

	// XAudioエンジンのインスタンスを生成
	result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if FAILED(result) {
		assert(0);
	}

	// マスターボイスを生成
	result = xAudio2_->CreateMasteringVoice(&masterVoice);
	if FAILED(result) {
		assert(0);
	}
}

void Sound::LoadWave(const std::string& filename, const bool& isConvert)
{
	std::string fullpath = directoryPath_ + filename;

	//同じ音をロードしていないか
	if (soundDatas_.find(fullpath) != soundDatas_.end()) { return; }

	//ファイル入力ストリームのインスタンス
	std::ifstream file;

	//wavファイルをバイナリモードで開く
	file.open(fullpath, std::ios_base::binary);
	assert(file.is_open());

	//RIFFヘッダの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//fileがRiffかチェック(先頭四文字をチェック)
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
	{
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0)
	{
		assert(0);
	}
	//元データがmp3だと×！（拡張子を変えてもダメ⇒コンバータで変換）


//Formatチャンクの読み込み
	FormatChunk format = {};
	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0)//"fmt "最後半角スペース！！！
	{
		assert(0);
	}

	//チャンク本体の読み込み
	if (format.chunk.size > sizeof(format.fmt))//18byteより大きかったら
	{
		int32_t size = format.chunk.size - sizeof(format.fmt);
		//18byte分読み取っておく
		file.read((char*)&format.fmt, sizeof(format.fmt));

		//仮のformat
		FormatChunk format2 = {};
		//その差分読み取って進める
		file.read((char*)&format2.fmt, size);
	}
	else
	{
		//assert(format.chunk.size <= sizeof(format.fmt));
		file.read((char*)&format.fmt, format.chunk.size);
		//16,18byteのフォーマットチャンクに対応
		//（40byteは×⇒40読み込んだ後で、先頭18byteだけコピーするといい）
	}

	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//junkチャンクを検出したら
	if (strncmp(data.id, "JUNK", 4) != isConvert)//引数によって変えられる
	{
		//junkチャンクの最後まで読み飛ばす(読み取りヘッドを動かす)
		file.seekg(data.size, std::ios_base::cur);
		//(チャンクヘッダを)再読み込み
		file.read((char*)&data, sizeof(data));
	}
	//本物のデータチャンク
	if (strncmp(data.id, "data", 4 != 0))
	{
		assert(0);
	}
	//Dataチャンクのデータ部（波形データの読み込み）
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);


	//waveファイルを閉じる
	file.close();


	//returnするための音声データ
	SoundData soundData = {};

	soundData.wfex = format.fmt;//波形のフォーマット
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);//波形データ
	soundData.bufferSize = data.size;//波形データのサイズ

	//サウンドデータを連想配列に格納(複製してセットでマップに格納)
	soundDatas_.insert(std::make_pair(fullpath, soundData));
}

void Sound::UnLoad(SoundData* soundData)
{
	//バッファのメモリ開放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void Sound::PlayWave(const std::string& filename, const float& volume, const bool& Loop)
{
	HRESULT result;

	std::string fullpath = directoryPath_ + filename;

	//ファイル名から探す
	std::map < std::string, Sound::SoundData>::iterator it
		= soundDatas_.find(fullpath);
	//未読み込みの検出
	assert(it != soundDatas_.end());
	//サウンドデータの参照を取得
	SoundData& soundData = it->second;

	//波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	//ループ再生
	if (Loop == true) { buf.LoopCount = XAUDIO2_LOOP_INFINITE; }

	//波形データの再生&設定
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->SetVolume(volume);
	result = pSourceVoice->Start();

	//停止用
	soundData.pSourceVoice.push_back(pSourceVoice);
}

void Sound::StopWave(const std::string& filename)
{
	std::string fullpath = directoryPath_ + filename;

	//ファイル名から探す
	std::map < std::string, Sound::SoundData>::iterator it
		= soundDatas_.find(fullpath);
	//未読み込みの検出
	assert(it != soundDatas_.end());

	//そのファイル名があれば
	SoundData& soundData_ = it->second;

	if (soundData_.pSourceVoice.size() > 0)
	{
		for (IXAudio2SourceVoice* source : soundData_.pSourceVoice)
		{
			//再生中の同じ名前のをすべて止める
			if (source != nullptr) { source->Stop(); }
		}

		//vectorを空にする
		soundData_.pSourceVoice.clear();
	}
}
Sound& Sound::GetInstance()
{
	static Sound inst; // private なコンストラクタを呼び出す。
	return inst;
}

//void SetpSourceVoice(SoundData& soundData, IXAudio2SourceVoice* pSourceVoice)
//{
//    soundData.pSourceVoice = pSourceVoice;
//}
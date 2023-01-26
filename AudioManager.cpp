#include "AudioManager.h"

Microsoft::WRL::ComPtr<IXAudio2>AudioManager::xAudio2_;
IXAudio2MasteringVoice* AudioManager::masterVoice_;

AudioManager::~AudioManager()
{
	// xaudio2の解放
	xAudio2_.Reset();
	//バッファのメモリを解放
	delete[] soundData_.pBuffer;

	soundData_.pBuffer = 0;
	soundData_.bufferSize = 0;
	soundData_.wfex = {};
}

void AudioManager::StaticInitialize()
{

	HRESULT result;
	result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	result = xAudio2_->CreateMasteringVoice(&masterVoice_);

}

//音声データの読み込み
void AudioManager::SoundLoadWave(const char* filename) {

	HRESULT result;

	//-------①ファイルオープン-------//

	//ファイル入力ストリームのインスタンス
	std::ifstream file;
	//.wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	//ファイルオープン失敗を検出する
	assert(file.is_open());

	//-------②.wavデータ読み込み-------//

	//RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));

	//ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}

	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	//Formatチャンクの読み込み
	FormatChunk format = {};

	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}

	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));

	//bextチャンクを検出した場合
	if (strncmp(data.id, "bext", 4) == 0) {
		//読み込み位置をJUNKチャンクの終わるまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}

	//JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		//読み込み位置をJUNKチャンクの終わるまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}

	//JUNKチャンクを検出した場合
	if (strncmp(data.id, "junk", 4) == 0) {
		//読み込み位置をJUNKチャンクの終わるまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}
	//LISTチャンクを検出した場合
	if (strncmp(data.id, "LIST", 4) == 0) {
		//読み込み位置をJUNKチャンクの終わるまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	//Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	//Waveファイルを閉じる
	file.close();

	//-------③読み込んだ音声データをreturn-------//

	//returnする為の音声データ
	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	soundData_ = soundData;

}

//-------音声データの解放-------//
void AudioManager::SoundUnload() {
	// xaudio2の解放
	xAudio2_.Reset();




}

//------サウンドの再生-------//

//音声再生
void AudioManager::SoundPlayWave(bool loop, float volume) {
	HRESULT result;

	if (isPlay) {
		return;
	}

	isPlay = true;

	//波形フォーマットを元にSourceVoiceの生成
	result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData_.wfex);
	assert(SUCCEEDED(result));

	//再生する波形データの設定
	buf.pAudioData = soundData_.pBuffer;
	buf.AudioBytes = soundData_.bufferSize;

	pSourceVoice->SetVolume(volume);

	if (loop)
	{
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	buf.Flags = XAUDIO2_END_OF_STREAM;


	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();

}

//----------音声ストップ------------//
void AudioManager::StopWave()
{
	HRESULT result;
	if (pSourceVoice != nullptr)
	{
		result = pSourceVoice->Stop();
		result = pSourceVoice->FlushSourceBuffers();
		result = pSourceVoice->SubmitSourceBuffer(&buf);

		isPlay = false;
	}
}
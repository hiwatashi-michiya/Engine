#pragma once
#include <array>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <string>
#include <unordered_map>
#include <wrl.h>
#include <xaudio2.h>

/// <summary>
/// オーディオの再生管理をするクラス
/// </summary>
class AudioManager
{
public:
	//インスタンス取得
	static AudioManager* GetInstance();

	//初期化
	void Initialize();

	//終了処理
	void Finalize();
	//XAudio2取得
	IXAudio2* GetXAudio2() { return xAudio2_.Get(); }

	//チャンクヘッダ
	struct ChunkHeader
	{
		std::array<char, 4> id; //チャンク毎のID
		int32_t size; //チャンクサイズ
	};

	//RIFFヘッダチャンク
	struct RiffHeader
	{
		ChunkHeader chunk; //"RIFF"
		std::array<char, 4> type; //"WAVE"
	};

	//FMTチャンク
	struct FormatChunk
	{
		ChunkHeader chunk; //"fmt"
		WAVEFORMATEX fmt; //波形フォーマット
	};

	//音声データ
	struct SoundData
	{
		//波形フォーマット
		WAVEFORMATEX* wfex;
		//バッファの先頭アドレス
		BYTE* pBuffer;
		//バッファのサイズ
		unsigned int bufferSize;

	};

	//サウンドデータとソースボイスを纏めた構造体
	struct Audio
	{
		WAVEFORMATEX* wfex;
		bool isPause = false;
		IXAudio2SourceVoice* pSourceVoice = nullptr;
		IMFSourceReader* pMFSourceReader{ nullptr };
		IMFMediaType* pMFMediaType{ nullptr };
		std::vector<BYTE> mediaData;
	};

	//音声データ読み込み
	/*uint32_t SoundLoadWave(const char* filename);*/

	//Media Foundationで読み込み
	uint32_t LoadInMF(const std::string& filename);

	/// <summary>
	/// 音声再生
	/// </summary>
	/// <param name="handle">ハンドル</param>
	/// <param name="volume">音量。0.0f～1.0f</param>
	/// <param name="isLoop">ループするかどうか</param>
	void Play(uint32_t handle, float volume, bool isLoop = false);

	/// <summary>
	/// 音声停止
	/// </summary>
	/// <param name="handle">ハンドル</param>
	void Stop(uint32_t handle);

	/// <summary>
	/// 一時停止
	/// </summary>
	/// <param name="handle">ハンドル</param>
	void Pause(uint32_t handle);

	/// <summary>
	/// 停止したところから再生
	/// </summary>
	/// <param name="handle">ハンドル</param>
	void ReStart(uint32_t handle);

	/// <summary>
	/// ボリューム調整
	/// </summary>
	/// <param name="handle">ハンドル</param>
	/// <param name="volume">音量</param>
	void SetVolume(uint32_t handle, float volume);

	/// <summary>
	/// 再生中かどうか
	/// </summary>
	/// <param name="handle">ハンドル</param>
	/// <returns>再生中だったらtrue</returns>
	bool IsPlaying(uint32_t handle);

private:

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;
	//ファイル名と数字を紐づけたマップ
	std::unordered_map<std::string, uint32_t> intMap_;
	//オーディオリスト
	std::vector<Audio> audios_;
	//現在のオーディオ数
	static uint32_t audioCount_;
	//最大オーディオ数
	static const uint32_t kMaxAudioCount_ = 256;

private:
	//シングルトン化
	AudioManager() = default;
	~AudioManager() = default;
	AudioManager(const AudioManager&) = delete;
	const AudioManager& operator=(const AudioManager&) = delete;

};


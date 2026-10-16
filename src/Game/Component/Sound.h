#pragma once
//===========================================================================
//! @file Sound.h
//! @brief 音再生のコンポーネント
//===========================================================================

//---------------------------------------------------------------------------
//! @class Sound
//! @brief 音再生を管理するクラス
//---------------------------------------------------------------------------
class Sound : public Component
{
public:
    BP_COMPONENT_DECL(Sound, u8"音再生コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 描画
    void Draw() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

    //----------------------------------------------------------
    //! @enum     Type
    //! @brief    音の種類
    //----------------------------------------------------------
    enum TYPE
    {
        SE    = 0,   //!< 効果音
        MUSIC = 1,   //!< 音楽
    };

    //----------------------------------------------------------
    //! @struct   Desc
    //! @brief 音の情報
    //----------------------------------------------------------
    struct Desc
    {
        std::string name_;          //!< 音名
        std::string file_path_;     //!< ファイルパス
        float radius_ = 256.0f;     //!< 3Dサウンドの半径
        float volume_ = 1.0f;       //!< 音量
        TYPE type_    = TYPE::SE;   //!< 音の種類
    };

    //! @brief 音データを読み込む
    //! @param sounds 読み込む音のリスト(音名、ファイルパス、半径 = 256.0f、音量 = 1.0f、種類 0= SE、1 = MUSIC)
    void Load(const std::vector<Desc>& sounds);

    //! @brief 音データを読み込む
    //! @param sounds 読み込む音のリスト(音名、ファイルパス、半径 = 256.0f(使わない)、音量 = 1.0f、種類 0= SE、1 = MUSIC)
    void Load2D(const std::vector<Desc>& sounds);

    //! @brief 音を再生
    //! @param name 音名
    //! @param playtype 再生タイプ(DX_PLAYTYPE_BACK / DX_PLAYTYPE_LOOP / DX_PLAYTYPE_NORMAL)
    void Play(std::string name, int playtype = DX_PLAYTYPE_BACK);

    //! @brief 音を再生
    //! @param name 音名
    //! @param playtype 再生タイプ(DX_PLAYTYPE_NORMAL / DX_PLAYTYPE_BACK / DX_PLAYTYPE_LOOP)
    void Play2D(std::string name, int playtype = DX_PLAYTYPE_BACK);

    //! @brief 音を再生
    //! @brief 音が再生している場合は再生しない
    //! @param name 再生するサウンド名
    //! @param playtype 再生タイプ(DX_PLAYTYPE_NORMAL / DX_PLAYTYPE_BACK / DX_PLAYTYPE_LOOP)
    void PlayNoSame(std::string name, int playtype = DX_PLAYTYPE_BACK);

    //! @brief 音を再生
    //! @brief 音が再生している場合は再生しない
    //! @param name 再生するサウンド名
    //! @param playtype 再生タイプ(DX_PLAYTYPE_NORMAL / DX_PLAYTYPE_BACK / DX_PLAYTYPE_LOOP)
    void PlayNoSame2D(std::string name, int playtype = DX_PLAYTYPE_BACK);

    //! @brief 音を再生
    //! @brief 同じサウンドなら再生しない
    //! @param name 再生するサウンド名
    //! @param playtype 再生タイプ(DX_PLAYTYPE_NORMAL / DX_PLAYTYPE_BACK / DX_PLAYTYPE_LOOP)
    void PlayOnce(std::string name, int playtype = DX_PLAYTYPE_BACK);

    //! @brief 音を再生
    //! @brief 同じサウンドなら再生しない
    //! @param name 再生するサウンド名
    //! @param playtype 再生タイプ(DX_PLAYTYPE_NORMAL / DX_PLAYTYPE_BACK / DX_PLAYTYPE_LOOP)
    void PlayOnce2D(std::string name, int playtype = DX_PLAYTYPE_BACK);

    //! @brief 音再生を止める
    //! @param name 音名
    void Stop(std::string name);

    //! @brief 再生中か
    //! @retval true 再生中
    //! @retval false 再生ではない
    bool IsPlaying();

    //! @brief サウンドの再生速度を設定
    //! @param speed 設定する速度倍率
    void SetSpeed(int speed);

    //! @brief ポーズに合わせて音を一時停止と再開
    static void UpdateMenuPause();

private:
    //! @brief メニューで一時停止するか
    //! @param paused 一時停止するかどうか
    void SetMenuPaused(bool paused);

    std::vector<int> handle_;                              //!< サウンドハンドル
    std::unordered_map<int, int> play_types_;              //!< ハンドルの再生形式
    std::unordered_map<int, int> paused_sounds_;           //!< メニューで一時停止した音と再生形式
    std::unordered_map<std::string, int> sound_data_;      //!< サウンドのデータ
    std::unordered_map<std::string, float> radius_data_;   //!< サウンドの半径データ
    std::unordered_map<std::string, float> volume_data_;   //!< サウンドの音量データ
    std::unordered_map<std::string, TYPE> type_data_;      //!< サウンドの種類データ
    int current_handle_ = 0;                               //!< 現在再生している音
    bool use_debug_     = false;                           //!< デバッグ表示するか
};

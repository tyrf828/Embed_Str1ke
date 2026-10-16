#pragma once
//===========================================================================
//! @file SceneGameManager.h
//! @brief ゲームシーンマネージャー
//===========================================================================

USING_PTR(SpawnManager);
USING_PTR(SceneGameManager);
USING_PTR(StageTrigger);
USING_PTR(AutoDrawText);
USING_PTR(DialogueVoice);
USING_PTR(Player);

//---------------------------------------------------------------------------
//! @class SceneGameManager
//! @brief ゲームシーンマネージャー
//---------------------------------------------------------------------------
class SceneGameManager : public Object
{
public:
    BP_OBJECT_DECL(SceneGameManager, "SceneGameManager");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 描画
    void Draw() override;

    //! @brief 遅延描画
    void LateDraw() override;

    //! @brief 終了
    void Exit() override;

    //! @brief ボス壁を設定
    //! @param boss_wall ボス壁
    //! @return 自身のポインタ
    SceneGameManagerPtr SetBossWall(ObjectPtr boss_wall)
    {
        boss_wall_ = boss_wall;
        return std::dynamic_pointer_cast<SceneGameManager>(shared_from_this());
    }

    //! @brief スポナーを追加
    //! @param spawner スポナー
    //! @return 自身のポインタ
    SceneGameManagerPtr AddSpawner(SpawnManagerPtr spawner)
    {
        spawners_.push_back(spawner);
        return std::dynamic_pointer_cast<SceneGameManager>(shared_from_this());
    }

    //! @brief プレイヤーを設定
    //! @param player プレイヤー
    //! @return 自身のポインタ
    SceneGameManagerPtr SetPlayer(PlayerPtr player)
    {
        player_ = player;
        return std::dynamic_pointer_cast<SceneGameManager>(shared_from_this());
    }

private:
    ObjectPtr boss_wall_               = nullptr;       //!< ボス壁
    StageTriggerPtr enter_boss_checker = nullptr;       //!< ボスエリア侵入判定
    PlayerPtr player_                  = nullptr;       //!< プレイヤー
    SpawnManagerPtrVec spawners_;                       //!< スポナーリスト
    float quest_timer_                      = 0.0f;     //!< クエストタイマー
    static constexpr float QUEST_CLEAR_TIME = 120.0f;   //!< クエストクリア時間
    int text_box_ghandle_                   = -1;       //!< テキストボックス画像ハンドル
    float text_box_scale_                   = 0.0f;     //!< テキストボックススケール

    bool is_boss_guide_close_  = false;              //!< ボスガイドが閉じているか
    bool is_boss_guide_finish_ = false;              //!< ボスガイドが終了しているか
    DialogueVoicePtr dialogue_voice_;                //!< 会話音声
    AutoDrawTextPtr text_ = nullptr;                 //!< テキスト描画
    std::vector<std::string> guide_texts_;           //!< ガイドテキスト
    float text_elapsed_time_               = 0.0f;   //!< テキスト経過時間
    static constexpr float TEXT_STAY_TIME_ = 2.0f;   //!< テキスト表示時間
    int current_guide_                     = 0;      //!< 現在のガイドインデックス
    void BossNavigationGudie();                      //!< ボスへのナビゲーションガイド
    ObjectPtr arrow_ = nullptr;                      //!< 矢印
    void DrawNaviArrow();                            //!< ナビゲーション矢印描画
    ObjectPtr boss_show_camera_  = nullptr;          //!< ボス場所表示カメラ
    float boss_show_elapsed_     = 0.0f;             //!< ボス表示経過時間
    bool boss_show_timer_active_ = false;            //!< ボス表示タイマー有効フラグ
};

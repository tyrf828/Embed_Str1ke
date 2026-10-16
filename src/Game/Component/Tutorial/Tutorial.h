#pragma once
//===========================================================================
//! @file Tutorial.h
//! @brief チュートリアルコンポーネント
//===========================================================================
#include <System/Component/Component.h>
#include <Game/Component/DialogueVoice.h>

USING_PTR(Tutorial);
USING_PTR(TutorialBase);
USING_PTR(Player);
USING_PTR(Camera);
USING_PTR(AutoDrawText);

//---------------------------------------------------------------------------
//! @class TutorialBase
//! @brief チュートリアルベースクラス
//---------------------------------------------------------------------------
class TutorialBase : public Component
{
public:
    //! @brief チュートリアル情報
    struct ProtectInfo
    {
        ObjectPtr owner_;               //!< オーナーオブジェクト
        PlayerWeakPtr player_;          //!< プレイヤ
        CameraWeakPtr camera_;          //< カメラ
        int message_box_handle_ = -1;   //!< メッセージボックスハンドル
        int navi_handle_        = -1;   //< オペレーター画像ハンドル
        int box_size_buffer_x_;         //< ボックスサイズバッファX
        int box_size_buffer_y_;         //< ボックスサイズバッファY
        ObjectPtr tutorail_wall_;       //< チュートリアル用壁オブジェクト
    };

    //! @brief 初期化
    virtual void Init(const ProtectInfo& info)
    {
        // チュートリアル情報を保存
        info_           = info;
        dialogue_voice_ = info_.owner_->AddComponent<DialogueVoice>();
    }
    //! @brief 更新
    virtual void Update()
    {
        // メッセージキー入力判定
        is_press_message_key_ = false;
        if(IsKeyOn(KEY_INPUT_RETURN) || IsKeyOn(KEY_INPUT_SPACE) || IsPadOn(PAD_ID::PAD_A))
        {
            is_press_message_key_ = true;
        }
    }
    //! @brief 遅延描画
    virtual void LateDraw() {}
    //! @brief GUI表示処理
    virtual void GUI() {}
    //! @brief 終了
    virtual void Exit()
    {
        if(dialogue_voice_)
        {
            dialogue_voice_->Stop();
            info_.owner_->RemoveComponent(dialogue_voice_);
            dialogue_voice_.reset();
        }
        // すべてのテキストを削除
        DeleteAllText();
    }

    //! @brief チュートリアルステップを変更
    //! @tparam T 次のチュートリアルステップのクラス
    template <typename T> void ChangeTutorialStep();

protected:
    //! @brief クエストボックスを描画
    //! @param box_scale ボックスのスケール
    void DrawQuestBox(float2 box_scale);

    //! @brief クエストメッセージボックスを描画
    //! @param box_scale ボックスのスケール
    void DrawQuestMessageBox(float2 box_scale);

    //! @brief クエストボックスのアニメーション
    //! @param is_close 閉じるアニメーションか
    void QuestBoxAnimation(bool is_close);

    //! @brief 全てのテキストを削除
    void DeleteAllText();

    //! @brief ナビオペレーターを描画
    void DrawNaviOperator();

    //! @brief クエストクリア処理
    //! @param text_ptr 表示するテキスト
    //! @param close_time テキストボックスを閉じるまでの時間
    void QuestClear(AutoDrawTextPtr text_ptr, float close_time = 2.0f);

    //! @brief プレイヤー操作を停止
    //! @param is_stop 停止するか
    void StopPlayerControl(bool is_stop);

    //! @brief メインテキストボックスのアニメーション
    //! @param max_scale_y 最大スケールY値
    void MainTextBoxAnimation(float max_scale_y = 1.0f);

    //! @brief メッセージ入力処理
    //! @param message_text メッセージテキストコンポーネント
    //! @param current_message_index 現在のメッセージインデックス
    //! @param text_speed テキストの表示速度
    void ProcessMessageInput(AutoDrawTextPtr& message_text, int& current_message_index, float text_speed);

    //! @brief メッセージキーが押されたか
    //! @retval true 押された
    //! @retval false 押されなかった
    bool IsPressMessageKey() const
    {
        return is_press_message_key_;
    }

    DialogueVoicePtr dialogue_voice_;                                            //!< 会話音声
    ProtectInfo info_;                                                           //!< チュートリアル情報
    std::vector<std::string> messages_;                                          //!< メッセージ
    float2 message_box_top_left_       = {WINDOW_W * 0.23f, WINDOW_H * 0.8f};    // メッセージボックス左上座標
    float2 message_box_bottom_right_   = {WINDOW_W * 0.77f, WINDOW_H * 0.93f};   // メッセージボックス右下座標
    float2 quest_box_scale_            = {0.5f, 0.0f};                           // クエストボックスのスケール
    float2 text_box_scale_             = {0.8f, 0.7f};                           // テキストボックスのスケール
    const float2 QUEST_BOX_MAX_        = {0.5f, 1.0f};                           // クエストボックス最大スケール
    const float2 SUB_TEXT_BOX_MAX_     = {0.8f, 0.7f};                           // サブテキストボックス最大スケール
    bool is_close_text_box_            = false;                                  // テキストボックスを閉じるか
    float quest_elasped_time_          = 0.0f;                                   // クエスト表示経過時間
    float clear_elasped_time_          = 0.0f;                                   // クエストクリア表示経過時間
    const int CLEAR_COLOR_             = GetColor(0, 255, 0);                    // クエストクリアカラー
    bool draw_quest_ghandle_           = false;                                  // クエストグラフィックハンドルを描画するか
    static constexpr float TEXT_SPEED_ = 0.05f;                                  // テキストの表示速度
    bool is_press_message_key_         = false;                                  // メッセージキーが押されたか
};

//---------------------------------------------------------------------------
//! @class Tutorial
//! @brief チュートリアルコンポーネント
//---------------------------------------------------------------------------
class Tutorial : public Component
{
public:
    BP_COMPONENT_DECL(Tutorial, u8"チュートリアル");

    //! @brief 初期化
    void Init() override;
    //! @brief 更新
    void Update() override;
    //! @brief 遅延描画
    void LateDraw() override;
    //! @brief GUI表示処理
    void GUI() override;
    //! @brief 終了
    void Exit() override;

    //! @brief 状態を設定
    void SetState(std::shared_ptr<TutorialBase> state)
    {
        if(now_state_)
        {
            now_state_->Exit();
        }
        now_state_ = state;
        if(now_state_)
        {
            info_.owner_ = GetOwnerPtr();
            now_state_->Init(info_);
        }
    }

    //! @brief プレイヤーを設定
    TutorialPtr SetPlayer(PlayerPtr player)
    {
        info_.player_ = player;
        return dynamic_pointer_cast<Tutorial>(shared_from_this());
    }

    //! @brief カメラを設定
    TutorialPtr SetCamera(CameraPtr camera)
    {
        info_.camera_ = camera;
        return dynamic_pointer_cast<Tutorial>(shared_from_this());
    }

private:
    std::shared_ptr<TutorialBase> now_state_ = nullptr;   //!< 現在のチュートリアル状態
    TutorialBase::ProtectInfo info_;                      //!< チュートリアル情報
};

//! @brief チュートリアルステップを変更
//! @tparam T 次のチュートリアルステップのクラス
template <typename T> void TutorialBase::ChangeTutorialStep()
{
    if(auto owner = info_.owner_)
    {
        if(auto cutscene = owner->GetComponent<Tutorial>())
        {
            cutscene->SetState(std::make_shared<T>());
        }
    }
}

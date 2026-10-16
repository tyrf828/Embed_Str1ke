#pragma once
//===========================================================================
//! @file CutSceneStageFade.h
//! @brief ステージが消えるカットシーン
//===========================================================================
#include "Game/Component/CutScene/CutScene.h"

USING_PTR(AutoDrawText);
USING_PTR(DialogueVoice);

//----------------------------------------------------------
//! @class CutSceneStageFade
//! @brief ステージが消えるカットシーン
//----------------------------------------------------------
class CutSceneStageFade : public CutSceneBase
{
public:
    BP_COMPONENT_DECL(CutSceneStageFade, u8"ステージが消える");

    //! @brief 初期化
    //! @param info カットシーンの保護情報
    void Init(const ProtectInfo& info) override;

    //! @brief 更新
    void Update() override;

    //! @brief 遅延描画
    void LateDraw() override;

    //! @brief GUI表示
    void GUI() override;

    //! @brief 終了処理
    void Exit() override;

private:
    float3 aim_start_pos_                     = {20.4f, 38.1f, 39.7f};   //!< カメラの開始位置
    float3 aim_end_pos_                       = {-43.2f, 38.1f, 9.0f};   //!< カメラの終了位置
    float3 initial_look_target_               = {0.0f, 0.0f, 0.0f};      //!< 初期の注視点
    static constexpr float ROTATION_DURATION_ = 2.0f;                    //!< カメラの移動にかかる時間
    //----------------------------------------------------------
    //! @enum CameraState
    //! @brief カメラの状態
    //----------------------------------------------------------
    enum CameraState
    {
        TO_START,   //!< 開始位置へ移動
        TO_END,     //!< 終了位置へ移動
        TO_INIT,    //!< 初期位置へ移動
        COMPLETE    //!< 完了
    };
    CameraState camera_state_ = CameraState::TO_START;   //!< カメラの状態

    DialogueVoicePtr dialogue_voice_;       //!< 会話音声
    AutoDrawTextPtr draw_text_ = nullptr;   //!< テキスト描画コンポーネント
    std::vector<std::string> messages_;     //!< メッセージリスト
    int current_message_index_ = 0;         //!< 現在のメッセージインデックス

    //! @brief 次のテキストへ移動
    void MoveNextText();
};

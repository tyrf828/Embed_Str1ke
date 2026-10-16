#pragma once
//===========================================================================
//! @file CutSceneEscape.h
//! @brief プレイヤーが脱出したカットシーン
//===========================================================================
#include "Game/Component/CutScene/CutScene.h"

USING_PTR(AutoDrawText);
USING_PTR(DialogueVoice);

//----------------------------------------------------------
//! @class CutSceneEscape
//! @brief プレイヤーが脱出したカットシーン
//----------------------------------------------------------
class CutSceneEscape : public CutSceneBase
{
public:
    BP_COMPONENT_DECL(CutSceneEscape, u8"プレイヤーが脱出した");

    //! @brief 初期化
    //! @param info カットシーンの保護情報
    void Init(const ProtectInfo& info) override;

    //! @brief 更新
    void Update() override;

    //! @brief GUI表示
    void GUI() override;

    //! @brief 終了処理
    void Exit() override;

private:
    //----------------------------------------------------------
    //! @enum State
    //! @brief カットシーンの状態
    //----------------------------------------------------------
    enum State
    {
        LOOK_UP,     //!< 上を見る
        CLIMB_UP,    //!< 登る
        TRUCK_RUN,   //!< トラックが走る
        LOOK_BACK,   //!< 振り返る
        COMPLETED,   //!< 完了
    };
    State state_            = State::LOOK_UP;             //!< 現在の状態
    float3 camera_prev_rot_ = float3{0.0f, 0.0f, 0.0f};   //!< 前回のカメラ回転
    float3 camera_look_pos_ = float3{0.0f, 0.0f, 0.0f};   //!< カメラの注視点

    float3 camera_in_truck_pos_ = float3{0.0f, 0.0f, 0.0f};   //!< トラック内のカメラ位置
    float3 camera_in_truck_rot_ = float3{0.0f, 0.0f, 0.0f};   //!< トラック内のカメラ回転

    DialogueVoicePtr dialogue_voice_;       //!< 会話音声
    AutoDrawTextPtr draw_text_ = nullptr;   //!< テキスト描画コンポーネント
};

#pragma once
//===========================================================================
//! @file CutSceneRunEscape.h
//! @brief プレイヤーが出口に逃げているカットシーン
//===========================================================================
#include "Game/Component/CutScene/CutScene.h"

USING_PTR(AutoDrawText);
USING_PTR(DialogueVoice);

//----------------------------------------------------------
//! @class CutSceneRunEscape
//! @brief プレイヤーが出口に逃げているカットシーン
//----------------------------------------------------------
class CutSceneRunEscape : public CutSceneBase
{
public:
    BP_COMPONENT_DECL(CutSceneRunEscape, u8"プレイヤーが出口に逃げている");

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
    ObjectPtr player_fps_body_             = nullptr;                    //!< FPS視点のプレイヤー
    float3 look_target_                    = float3{0.0f, 0.0f, 0.0f};   //!< カメラの注視点
    static constexpr float HEAD_NOD_SPEED_ = 8.0f;                       //!< 頭の揺れ速度

    DialogueVoicePtr dialogue_voice_;       //!< 会話音声
    AutoDrawTextPtr draw_text_ = nullptr;   //!< テキスト描画コンポーネント
};

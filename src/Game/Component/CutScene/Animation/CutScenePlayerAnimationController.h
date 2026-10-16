#pragma once
//===========================================================================
//! @file CutScenePlayerAnimationController.h
//! @brief 演出プレイヤーのアニメーションコントローラー
//===========================================================================
#include <Game/Component/AnimationControllerBase.h>

//---------------------------------------------------------------------------
//! @class CutScenePlayerAnimationController
//! @brief 演出プレイヤーアニメーションコンポーネント
//---------------------------------------------------------------------------
class CutScenePlayerAnimationController : public AnimationControllerBase
{
public:
    BP_COMPONENT_DECL(CutScenePlayerAnimationController, u8"アニメーションコンポーネント");

    //----------------------------------------------------------
    //! @enum STATE
    //! @brief アニメーションの状態
    //----------------------------------------------------------
    enum struct STATE
    {
        IDLE,          //!< 待機
        WALK,          //!< 歩き
        ATTACK,        //!< 攻撃
        PICK_UP,       //!< つかむ
        LOOK_AROUND,   //!< 周りを見る
        RUN_BACK,      //!< 戻って走る
        RUN,           //!< 走る

        ANIM_MAX   //!< アニメーションの最大数
    };

    //----------------------------------------------------------
    //! @enum STATE_BIT
    //! @brief アニメーションの状態のビット
    //----------------------------------------------------------
    enum STATE_BIT
    {
        IDLE        = 1 << static_cast<u32>(STATE::IDLE),          //!< 待機
        WALK        = 1 << static_cast<u32>(STATE::WALK),          //!< 歩き
        ATTACK      = 1 << static_cast<u32>(STATE::ATTACK),        //!< 攻撃
        PICK_UP     = 1 << static_cast<u32>(STATE::PICK_UP),       //!< つかむ
        LOOK_AROUND = 1 << static_cast<u32>(STATE::LOOK_AROUND),   //!< 周りを見る
        RUN_BACK    = 1 << static_cast<u32>(STATE::RUN_BACK),      //!< 戻って走る
        RUN         = 1 << static_cast<u32>(STATE::RUN),           //!< 走る
    };

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief GUI表示処理
    void GUI() override;

private:
};

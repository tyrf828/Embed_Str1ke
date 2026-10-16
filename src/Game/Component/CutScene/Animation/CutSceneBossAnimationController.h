#pragma once
//===========================================================================
//! @file CutSceneBossAnimationController.h
//! @brief 演出ボスのアニメーションコントローラー
//===========================================================================
#include <Game/Component/AnimationControllerBase.h>

//---------------------------------------------------------------------------
//! @class CutSceneBossAnimationController
//! @brief アニメーションコンポーネント
//---------------------------------------------------------------------------
class CutSceneBossAnimationController : public AnimationControllerBase
{
public:
    BP_COMPONENT_DECL(CutSceneBossAnimationController, u8"アニメーションコンポーネント");

    //----------------------------------------------------------
    //! @enum STATE
    //! @brief アニメーションの状態
    //----------------------------------------------------------
    enum struct STATE
    {
        SIT,         //!< 座る
        STAND_UP,    //!< 立ち上がる
        JUMP,        //!< ジャンプ
        ATTACK,      //!< 攻撃
        KNOCKBACK,   //!< ノックバック

        ANIM_MAX   //!< アニメーションの最大数
    };

    //----------------------------------------------------------
    //! @enum STATE_BIT
    //! @brief アニメーションの状態のビット
    //----------------------------------------------------------
    enum STATE_BIT
    {
        SIT       = 1 << static_cast<u32>(STATE::SIT),         //!< 座る
        STAND_UP  = 1 << static_cast<u32>(STATE::STAND_UP),    //!< 立ち上がる
        JUMP      = 1 << static_cast<u32>(STATE::JUMP),        //!< ジャンプ
        ATTACK    = 1 << static_cast<u32>(STATE::ATTACK),      //!< 攻撃
        KNOCKBACK = 1 << static_cast<u32>(STATE::KNOCKBACK),   //!< ノックバック
    };

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief GUI表示処理
    void GUI() override;

    void PlayOnce(int anim_flag);

private:
};

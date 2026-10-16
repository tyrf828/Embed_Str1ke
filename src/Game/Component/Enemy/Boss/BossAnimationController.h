#pragma once
#pragma once
//===========================================================================
//! @file BossAnimationController.h
//! @brief ボスのアニメーション制御コントローラー
//===========================================================================
#include <Game/Component/AnimationControllerBase.h>

//---------------------------------------------------------------------------
//! @class BossAnimationController
//! @brief ボスのアニメーションコンポーネント
//---------------------------------------------------------------------------
class BossAnimationController : public AnimationControllerBase
{
public:
    BP_COMPONENT_DECL(BossAnimationController, u8"ボスのアニメーションコンポーネント");

    //----------------------------------------------------------
    //! @enum STATE
    //! @brief アニメーションの状態
    //----------------------------------------------------------
    enum struct STATE
    {
        IDLE,                //!< 待機
        READY,               //!< 戦闘準備
        WALK_FRONT,          //!< 前
        WALK_RIGHT,          //!< 右
        WALK_LEFT,           //!< 左
        WALK_BACK,           //!< 後ろ
        HIT,                 //!< 被弾
        ATTACK,              //!< 攻撃
        KICK,                //!< キック
        FALL_BACK,           //!< 後退
        JUMP,                //!< ジャンプ
        DASH_ATTACK,         //!< 突進攻撃
        BLAST,               //!< 爆発
        SLAM_ATTACK_RIGHT,   //!< 右手コンボ攻撃
        SLAM_ATTACK_LEFT,    //!< 左手コンボ攻撃
        COMBO_KICK,          //!< コンボキック
        CLAP,                //!< ハイタッチ
        KNOCKBACK,           //!< ノックバック
        DEATH,               //!< 死亡

        ANIM_MAX   //!< アニメーションの最大数
    };

    //----------------------------------------------------------
    //! @enum STATE_BIT
    //! @brief アニメーションの状態のビット
    //----------------------------------------------------------
    enum STATE_BIT
    {
        IDLE              = 1 << static_cast<u32>(STATE::IDLE),                //!< 待機
        READY             = 1 << static_cast<u32>(STATE::READY),               //!< 戦闘準備
        WALK_FRONT        = 1 << static_cast<u32>(STATE::WALK_FRONT),          //!< 前
        WALK_RIGHT        = 1 << static_cast<u32>(STATE::WALK_RIGHT),          //!< 右
        WALK_LEFT         = 1 << static_cast<u32>(STATE::WALK_LEFT),           //!< 左
        WALK_BACK         = 1 << static_cast<u32>(STATE::WALK_BACK),           //!< 後ろ
        HIT               = 1 << static_cast<u32>(STATE::HIT),                 //!< 被弾
        ATTACK            = 1 << static_cast<u32>(STATE::ATTACK),              //!< 攻撃
        KICK              = 1 << static_cast<u32>(STATE::KICK),                //!< キック
        FALL_BACK         = 1 << static_cast<u32>(STATE::FALL_BACK),           //!< 後退
        JUMP              = 1 << static_cast<u32>(STATE::JUMP),                //!< ジャンプ
        DASH_ATTACK       = 1 << static_cast<u32>(STATE::DASH_ATTACK),         //!< 突進攻撃
        BLAST             = 1 << static_cast<u32>(STATE::BLAST),               //!< 爆発
        SLAM_ATTACK_RIGHT = 1 << static_cast<u32>(STATE::SLAM_ATTACK_RIGHT),   //!< 右手コンボ攻撃
        SLAM_ATTACK_LEFT  = 1 << static_cast<u32>(STATE::SLAM_ATTACK_LEFT),    //!< 左手コンボ攻撃
        COMBO_KICK        = 1 << static_cast<u32>(STATE::COMBO_KICK),          //!< コンボキック
        CLAP              = 1 << static_cast<u32>(STATE::CLAP),                //!< ハイタッチ
        KNOCKBACK         = 1 << static_cast<u32>(STATE::KNOCKBACK),           //!< ノックバック
        DEATH             = 1 << static_cast<u32>(STATE::DEATH),               //!< 死亡
    };

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief ヒットアニメーションを再再生
    void PlayHitAgain();

private:
};

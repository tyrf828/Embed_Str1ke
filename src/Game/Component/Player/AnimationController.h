#pragma once
//===========================================================================
//! @file AnimationController.h
//! @brief プレイヤーのアニメーション制御コントローラー
//===========================================================================
#include <Game/Component/AnimationControllerBase.h>

//---------------------------------------------------------------------------
//! @class AnimationController
//! @brief アニメーションコンポーネント
//---------------------------------------------------------------------------
class AnimationController : public AnimationControllerBase
{
public:
    BP_COMPONENT_DECL(AnimationController, u8"アニメーションコンポーネント");

    //----------------------------------------------------------
    //! @enum STATE
    //! @brief アニメーションの状態
    //----------------------------------------------------------
    enum struct STATE
    {
        IDLE,              //!< 待機
        WALK,              //!< 歩き
        RUN,               //!< 走り
        ATTACK1,           //!< 軽攻撃1
        ATTACK2,           //!< 軽攻撃2
        ATTACK3,           //!< 軽攻撃3
        REGEN,             //!< 回復
        ULTIMATE_DASH,     //!< 必殺技の前動作
        ULTIMATE_FINISH,   //!< 必殺技の終わり
        SMALL_HIT,         //!< 小ダメージ
        BIG_HIT,           //!< 大ダメージ
        STAND_UP,          //!< 起き上がり
        DEATH,             //!< 死亡

        ANIM_MAX   //!< アニメーションの最大数
    };

    //----------------------------------------------------------
    //! @enum STATE_BIT
    //! @brief アニメーションの状態のビット
    //----------------------------------------------------------
    enum STATE_BIT
    {
        IDLE            = 1 << static_cast<u32>(STATE::IDLE),              //!< 待機
        WALK            = 1 << static_cast<u32>(STATE::WALK),              //!< 歩き
        RUN             = 1 << static_cast<u32>(STATE::RUN),               //!< 走り
        ATTACK1         = 1 << static_cast<u32>(STATE::ATTACK1),           //!< 軽攻撃1
        ATTACK2         = 1 << static_cast<u32>(STATE::ATTACK2),           //!< 軽攻撃2
        ATTACK3         = 1 << static_cast<u32>(STATE::ATTACK3),           //!< 軽攻撃3
        REGEN           = 1 << static_cast<u32>(STATE::REGEN),             //!< 回復
        ULTIMATE_DASH   = 1 << static_cast<u32>(STATE::ULTIMATE_DASH),     //!< 必殺技の前動作
        ULTIMATE_FINISH = 1 << static_cast<u32>(STATE::ULTIMATE_FINISH),   //!< 必殺技の終わり
        SMALL_HIT       = 1 << static_cast<u32>(STATE::SMALL_HIT),         //!< 小ダメージ
        BIG_HIT         = 1 << static_cast<u32>(STATE::BIG_HIT),           //!< 大ダメージ
        STAND_UP        = 1 << static_cast<u32>(STATE::STAND_UP),          //!< 起き上がり
        DEATH           = 1 << static_cast<u32>(STATE::DEATH),             //!< 死亡
    };

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief カメラが敵にロックする時ターゲットの方向を向くを設定
    //! @param is_face_target ターゲットの方向を向くか
    void SetFaceTarget(bool is_face_target = false);

    //! @brief カメラが敵にロックする時ターゲットの方向を向いているか
    //! @retval true 向いている
    //! @retval false 向いていない
    bool IsFaceTarget() const;

private:
    bool is_face_target_ = false;   //!< ターゲットの方向を向くか
};

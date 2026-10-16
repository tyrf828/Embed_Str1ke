#pragma once
//===========================================================================
//! @file CloseEnemyAnimationController.h
//! @brief 敵のアニメーション制御コントローラー
//===========================================================================
#include <Game/Component/AnimationControllerBase.h>

//---------------------------------------------------------------------------
//! @class CloseEnemyAnimationController
//! @brief 敵のアニメーションコンポーネント
//---------------------------------------------------------------------------
class CloseEnemyAnimationController : public AnimationControllerBase
{
public:
    BP_COMPONENT_DECL(CloseEnemyAnimationController, u8"敵のアニメーションコンポーネント");

    //----------------------------------------------------------
    //! @enum STATE
    //! @brief アニメーションの状態
    //----------------------------------------------------------
    enum struct STATE
    {
        IDLE,         //!< 待機
        WALK,         //!< 歩き
        WALK_RIGHT,   //!< 右歩き
        WALK_LEFT,    //!< 左歩き
        WALK_BACK,    //< 後ろ歩き
        ATTACK,       //!<攻撃
        HIT,          //!< 小ダメージ
        BIG_HIT,      //!< 大ダメージ
        STAND_UP,     //!< 起き上がり
        DEATH,        //!< 死亡

        ANIM_MAX   //!< アニメーションの最大数
    };

    //----------------------------------------------------------
    //! @enum STATE_BIT
    //! @brief アニメーションの状態のビット
    //----------------------------------------------------------
    enum STATE_BIT
    {
        IDLE       = 1 << static_cast<u32>(STATE::IDLE),         //!< 待機
        WALK       = 1 << static_cast<u32>(STATE::WALK),         //!< 歩き
        WALK_RIGHT = 1 << static_cast<u32>(STATE::WALK_RIGHT),   //!< 右歩き
        WALK_LEFT  = 1 << static_cast<u32>(STATE::WALK_LEFT),    //!< 左歩き
        WALK_BACK  = 1 << static_cast<u32>(STATE::WALK_BACK),    //!< 後ろ歩き
        ATTACK     = 1 << static_cast<u32>(STATE::ATTACK),       //!< 攻撃
        HIT        = 1 << static_cast<u32>(STATE::HIT),          //!< 小ダメージ
        BIG_HIT    = 1 << static_cast<u32>(STATE::BIG_HIT),      //!< 大ダメージ
        STAND_UP   = 1 << static_cast<u32>(STATE::STAND_UP),     //!< 起き上がり
        DEATH      = 1 << static_cast<u32>(STATE::DEATH),        //!< 死亡
    };

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 現在アニメーションをもう一回再生する
    void PlayAgain();

private:
};

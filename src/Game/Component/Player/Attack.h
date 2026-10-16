#pragma once
//===========================================================================
//! @file Attack.h
//! @brief プレイヤーの攻撃コンポーネント
//===========================================================================
#include <Game/Component/Player/AttackBase.h>

//---------------------------------------------------------------------------
//! @class Attack
//! @brief 攻撃コンポーネント
//---------------------------------------------------------------------------
class Attack : public AttackBase
{
public:
    BP_COMPONENT_DECL(Attack, u8"攻撃コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 終了
    void Exit() override;

private:
    static constexpr float COMBO_START_TIME_  = 1.72f;   //!< 次の攻撃に移行時間
    static constexpr float COMBO_LIMIT_TIME_  = 2.85f;   //!< コンボ攻撃の制限時間
    static constexpr int NEXT_ATTACK_STAMINA_ = 100;     //!< 次の攻撃のスタミナの消耗量
    static constexpr float ATTACK_START_TIME_ = 1.3f;    //!< 攻撃判定付与時間
    static constexpr float ATTACK_LIMIT_TIME_ = 1.9f;    //!< 攻撃判定消す時間
    static constexpr float END_TIME_          = 3.22f;   //!< 攻撃アニメションの終了時間
    static constexpr int ATTACK_POINT_        = 100;     //!< 攻撃力
};

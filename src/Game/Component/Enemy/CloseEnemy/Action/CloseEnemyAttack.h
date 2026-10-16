#pragma once
//===========================================================================
//! @file CloseEnemyAttack.h
//! @brief 敵の攻撃コンポーネント
//===========================================================================
#include <Game/Component/Enemy/ActionBase/ActionAttackBase.h>

//---------------------------------------------------------------------------
//! @class CloseEnemyAttack
//! @brief 敵の攻撃コンポーネント
//---------------------------------------------------------------------------
class CloseEnemyAttack : public ActionAttackBase
{
public:
    BP_COMPONENT_DECL(CloseEnemyAttack, u8"敵の攻撃コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

private:
    static constexpr float ATTACK_DISTANCE_   = 15.0f;   //!< 攻撃距離
    static constexpr float ATTACK_START_TIME_ = 1.32f;   //!< 攻撃判定の開始時間
    static constexpr float ATTACK_END_TIME_   = 1.93f;   //!< 攻撃判定の終了時間

    static constexpr int ATK_ = 100;   //!< 攻撃力

    bool is_attack_ = false;   //!< 攻撃中か
};

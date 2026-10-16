#pragma once
//===========================================================================
//! @file FarEnemyAttack.h
//! @brief 敵の攻撃コンポーネント
//===========================================================================
#include <Game/Component/Enemy/ActionBase/ActionAttackBase.h>

//---------------------------------------------------------------------------
//! @class FarEnemyAttack
//! @brief 敵の攻撃コンポーネント
//---------------------------------------------------------------------------
class FarEnemyAttack : public ActionAttackBase
{
public:
    BP_COMPONENT_DECL(FarEnemyAttack, u8"敵の攻撃コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 攻撃距離を取得
    //! @return 攻撃距離
    static constexpr float GetAttackDistance()
    {
        return ATTACK_DISTANCE_;
    }

private:
    static constexpr float ATTACK_DISTANCE_ = 70.0f;   //!< 攻撃距離
    static constexpr int ATK_               = 140;     //!< 攻撃力

    bool is_attack_         = false;   //!< 攻撃中か
    bool is_player_visible_ = false;   //!< プレイヤーが見えるか
};

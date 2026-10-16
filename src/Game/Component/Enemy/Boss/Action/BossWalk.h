#pragma once
//===========================================================================
//! @file BossWalk.h
//! @brief ボスの歩行コンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class BossWalk
//! @brief ボスの歩行コンポーネント
//---------------------------------------------------------------------------
class BossWalk : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(BossWalk, u8"ボスの歩行コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 攻撃距離を設定
    //! @param distance 攻撃距離
    void SetAttackDistance(float distance)
    {
        attack_distance_ = distance;
    }

private:
    float attack_distance_                   = 0.0f;   //!< 攻撃距離
    float dash_attack_timer_                 = 0.0f;   //!< 突進攻撃タイマー
    static constexpr float DASH_ATTACK_TIME_ = 2.0f;   //!< 突進攻撃までの時間
};

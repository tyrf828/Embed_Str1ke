#pragma once
//===========================================================================
//! @file ActionThreatBase.h
//! @brief 敵の威嚇アクション基底クラス
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class ActionThreatBase
//! @brief 敵の威嚇アクション基底クラス
//---------------------------------------------------------------------------
class ActionThreatBase : public EnemyActionBase
{
public:
    //! @brief 威嚇最大距離を取得
    //! @return 威嚇最大距離
    virtual float GetThreatMaxDistance() const = 0;

    //! @brief 威嚇最小距離を取得
    //! @return 威嚇最小距離
    virtual float GetThreatMinDistance() const = 0;

protected:
    int strafe_dir_          = 1;       //!< +1:左 -1:右
    float change_timer_      = 0.0f;    //!< 状態変更タイマー
    bool back_safe_distance_ = false;   //!< 安全距離まで後退するか
    float back_target_dist_  = 0.0f;    //!< 後退目標距離
    float prev_action_speed_ = 0;       //!< 前のアクションの速度
};

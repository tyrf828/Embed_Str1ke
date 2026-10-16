#pragma once
//===========================================================================
//! @file BossConfrontBackward.h
//! @brief ボスの後退コンポーネント
//===========================================================================
#include "BossConfront.h"

//---------------------------------------------------------------------------
//! @class BossConfrontBackward
//! @brief ボスの後退コンポーネント
//---------------------------------------------------------------------------
class BossConfrontBackward : public BossConfrontFactory
{
public:
    BP_COMPONENT_DECL(BossConfrontBackward, u8"ボスの後退コンポーネント");

    //! @brief 初期化
    void ConfrontInit() override;

    //! @brief 更新
    void ConfrontUpdate() override;

    //! @brief 終了
    void ConfrontExit() override;

private:
    float time_                        = 0.0f;   //!< 経過時間
    static constexpr float BACK_TIME_  = 2.0f;   //!< 後退する時間
    static constexpr float MOVE_SPEED_ = 0.2f;   //!< 移動速度
    float prev_move_speed_             = 0.0f;   //!< 前の移動速度
};

#pragma once
//===========================================================================
//! @file BossBlastEffect.h
//! @brief ボスの爆発攻撃エフェクト
//===========================================================================
#include "EffectBase.h"

USING_PTR(FloorBreakEffect);

class FloorBreakEffect : public EffectBase
{
public:
    BP_OBJECT_DECL(FloorBreakEffect, "FloorBreakEffect");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

private:
    static constexpr float MAX_DESTROY_TIME_ = 10.0f;   //!< 破壊までの最大時間
};

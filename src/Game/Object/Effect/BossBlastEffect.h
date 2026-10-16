#pragma once
//===========================================================================
//! @file BossBlastEffect.h
//! @brief ボスの爆発攻撃エフェクト
//===========================================================================
#include "EffectBase.h"

USING_PTR(BossBlastEffect);

//---------------------------------------------------------------------------
//! @class BossBlastEffect
//! @brief ボスの爆発攻撃エフェクト
//---------------------------------------------------------------------------
class BossBlastEffect : public EffectBase
{
public:
    BP_OBJECT_DECL(BossBlastEffect, "BossBlastEffect");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

private:
};

#pragma once
//===========================================================================
//! @file ShootLaserEffect.h
//! @brief 敵の銃撃エフェクト
//===========================================================================
#include "EffectBase.h"
USING_PTR(ShootLaserEffect);

//---------------------------------------------------------------------------
//! @class ShootLaserEffect
//! @brief 敵の銃撃エフェクト
//---------------------------------------------------------------------------
class ShootLaserEffect : public EffectBase
{
public:
    BP_OBJECT_DECL(ShootLaserEffect, "ShootLaserEffect");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

private:
};

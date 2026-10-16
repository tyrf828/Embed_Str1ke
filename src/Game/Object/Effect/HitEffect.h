#pragma once
//===========================================================================
//! @file HitEffect.h
//! @brief プレイヤー武器が敵に当たったエフェクト
//===========================================================================
#include "EffectBase.h"

USING_PTR(HitEffect);

//---------------------------------------------------------------------------
//! @class HitEffect
//! @brief プレイヤー武器が敵に当たったエフェクト
//---------------------------------------------------------------------------
class HitEffect : public EffectBase
{
public:
    BP_OBJECT_DECL(HitEffect, "HitEffect");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

private:
};

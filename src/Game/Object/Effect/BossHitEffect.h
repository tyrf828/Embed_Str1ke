#pragma once
//===========================================================================
//! @file BossHitEffect.h
//! @brief ボスがプレイヤーに当たったエフェクト
//===========================================================================
#include "EffectBase.h"

USING_PTR(BossHitEffect);

//---------------------------------------------------------------------------
//! @class BossHitEffect
//! @brief ボスがプレイヤーに当たったエフェクト
//---------------------------------------------------------------------------
class BossHitEffect : public EffectBase
{
public:
    BP_OBJECT_DECL(BossHitEffect, "BossHitEffect");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

private:
};

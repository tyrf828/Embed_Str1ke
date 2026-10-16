#pragma once
//===========================================================================
//! @file EnemyHitEffect.h
//! @brief 敵がプレイヤーに当たったエフェクト
//===========================================================================
#include "EffectBase.h"

USING_PTR(EnemyHitEffect);

//---------------------------------------------------------------------------
//! @class EnemyHitEffect
//! @brief 敵がプレイヤーに当たったエフェクト
//---------------------------------------------------------------------------
class EnemyHitEffect : public EffectBase
{
public:
    BP_OBJECT_DECL(EnemyHitEffect, "EnemyHitEffect");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

private:
};

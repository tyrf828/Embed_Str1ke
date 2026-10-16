#pragma once
//===========================================================================
//! @file FinishThunderEffect.h
//! @brief 必殺技のフィンニッシュエフェクト
//===========================================================================
#include "EffectBase.h"
USING_PTR(FinishThunderEffect);

//---------------------------------------------------------------------------
//! @class FinishThunderEffect
//! @brief 必殺技のフィンニッシュエフェクト
//---------------------------------------------------------------------------
class FinishThunderEffect : public EffectBase
{
public:
    BP_OBJECT_DECL(FinishThunderEffect, "FinishThunderEffect");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

private:
};

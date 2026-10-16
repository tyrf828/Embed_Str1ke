//===========================================================================
//! @file FinishThunderEffect.cpp
//! @brief 必殺技のフィンニッシュエフェクト
//===========================================================================
#include "FinishThunderEffect.h"
#include <System/Component/ComponentEffect.h>

//! @brief 初期化
//! @return 初期化済み
bool FinishThunderEffect::Init()
{
    Super::Init();
    // エフェクトコンポーネントを追加
    auto effect = AddComponent<ComponentEffect>("data/Game/Effect/LightningStrike.efkefc");
    effect->SetScaleAxisXYZ(float3{1.5f, 1.5f, 1.5f});
    effect->SetPlaySpeed(1.3f);
    Play();
    return true;
}

//! @brief 更新
void FinishThunderEffect::Update()
{
    Super::Update();

    AutoDestroy(false);
}

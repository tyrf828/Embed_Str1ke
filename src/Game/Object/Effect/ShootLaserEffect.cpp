//===========================================================================
//! @file ShootLaserEffect.cpp
//! @brief 敵の銃撃エフェクト
//===========================================================================
#include "ShootLaserEffect.h"
#include <System/Component/ComponentEffect.h>

//! @brief 初期化
//! @return 初期化済み
bool ShootLaserEffect::Init()
{
    Super::Init();
    SetName("ShootLaserEffect");
    // エフェクトコンポーネントを追加
    auto effect = AddComponent<ComponentEffect>("data/Game/Effect/laser.efkefc");
    effect->SetRotationAxisXYZ(float3{0.0f, 180.0f, 0.0f});
    effect->SetPlaySpeed(0.35f);
    SetScaleAxisXYZ(float3{2.0f, 2.0f, 1.5f});
    Play();
    return true;
}

//! @brief 更新
void ShootLaserEffect::Update()
{
    Super::Update();

    AutoDestroy(false);
}

//===========================================================================
//! @file BossBlastEffect.cpp
//! @brief ボスの爆発攻撃エフェクト
//===========================================================================
#include "BossBlastEffect.h"
#include <System/Component/ComponentEffect.h>

//! @brief 初期化
//! @return 初期化済み
bool BossBlastEffect::Init()
{
    Super::Init();

    SetName("BossBlastEffect");

    SetRotationAxisXYZ(float3{-90.0f, 0.0f, 0.0f});

    // エフェクトコンポーネントを追加
    auto effect = AddComponent<ComponentEffect>("data/Game/Effect/Blow5.efkefc");
    effect->SetScaleAxisXYZ({5.0f, 5.0f, 5.0f});
    effect->SetPlaySpeed(0.8f);
    Play();
    return true;
}

//! @brief 更新
void BossBlastEffect::Update()
{
    Super::Update();
}

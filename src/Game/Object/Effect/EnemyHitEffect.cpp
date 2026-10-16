//===========================================================================
//! @file EnemyHitEffect.cpp
//! @brief 敵がプレイヤーに当たったエフェクト
//===========================================================================
#include "EnemyHitEffect.h"
#include <System/Component/ComponentEffect.h>

//! @brief 初期化
//! @return 初期化済み
bool EnemyHitEffect::Init()
{
    Super::Init();
    SetName("EnemyHitEffect");
    // エフェクトコンポーネントを追加
    auto effect = AddComponent<ComponentEffect>("data/Game/Effect/Attack_Impact.efkefc");
    effect->SetScaleAxisXYZ({1.0f, 1.0f, 1.0f});
    Play();
    return true;
}

//! @brief 更新
void EnemyHitEffect::Update()
{
    Super::Update();

    AutoDestroy(false);
}

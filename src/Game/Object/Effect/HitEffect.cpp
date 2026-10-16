//===========================================================================
//! @file HitEffect.cpp
//! @brief プレイヤー武器が敵に当たったエフェクト
//===========================================================================
#include "HitEffect.h"
#include <System/Component/ComponentEffect.h>

//! @brief 初期化
//! @return 初期化済み
bool HitEffect::Init()
{
    Super::Init();
    SetName("HitEffect");
    // エフェクトコンポーネントを追加
    auto effect = AddComponent<ComponentEffect>("data/Game/Effect/hiteffect.efkefc");
    effect->SetScaleAxisXYZ({2.0f, 2.0f, 2.0f});
    effect->SetPlaySpeed(0.5f);
    return true;
}

//! @brief 更新
void HitEffect::Update()
{
    Super::Update();

    // エフェクトを再生
    LatePlay();
    // エフェクトの再生が終了場合解放
    AutoDestroy(true);
}

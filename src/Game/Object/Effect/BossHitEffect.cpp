//===========================================================================
//! @file BossHitEffect.cpp
//! @brief ボスがプレイヤーに当たったエフェクト
//===========================================================================
#include "BossHitEffect.h"
#include <System/Component/ComponentEffect.h>

//! @brief 初期化
//! @return 初期化済み
bool BossHitEffect::Init()
{
    Super::Init();
    SetName("BossHitEffect");
    // エフェクトコンポーネントを追加
    auto effect = AddComponent<ComponentEffect>("data/Game/Effect/Sword6.efkefc");
    effect->SetScaleAxisXYZ({2.0f, 2.0f, 2.0f});
    return true;
}

//! @brief 更新
void BossHitEffect::Update()
{
    Super::Update();

    // エフェクトを再生
    LatePlay();
    // エフェクトの再生が終了場合解放
    AutoDestroy(true);
}

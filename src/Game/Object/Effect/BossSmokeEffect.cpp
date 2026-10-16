#include "BossSmokeEffect.h"
#include <System/Component/ComponentEffect.h>

bool BossSmokeEffect::Init()
{
    Super::Init();
    auto effect = AddComponent<ComponentEffect>("data/Game/Effect/Boss/smoke.efkefc");
    effect->SetScaleAxisXYZ({2.0f, 2.0f, 2.0f});
    return true;
}

void BossSmokeEffect::Update()
{
    Super::Update();
    // 座標はオブジェクト側、拡大率はエフェクト側で設定し、座標の二重適用を防ぐ。
    LatePlay();
    AutoDestroy(true);
}

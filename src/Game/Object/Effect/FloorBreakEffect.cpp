#include "FloorBreakEffect.h"
#include <Game/Component/Decal.h>

bool FloorBreakEffect::Init()
{
    Super::Init();
    SetScaleAxisXYZ(float3{100.0f, 100.0f, 100.0f});
    auto decal = AddComponent<Decal>("data/Shader/Texture/break.png");

    // 一定時間後に破棄する
    Destroy(MAX_DESTROY_TIME_);

    return true;
}

void FloorBreakEffect::Update()
{
    Super::Update();

    if(auto decal = GetComponent<Decal>())
    {
        // デカールの透明度を徐々に減らす
        float alpha = destroy_timer_ / MAX_DESTROY_TIME_;
        decal->SetAlpha(alpha);
    }
}

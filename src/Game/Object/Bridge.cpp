#include "Bridge.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionModel.h>

bool Bridge::Init()
{
    Super::Init();
    AddComponent<ComponentModel>("data/Game/Model/Bridge/Highway Bridge 1.mv1");
    auto model_col = AddComponent<ComponentCollisionModel>();
    model_col->AttachToModel();
    model_col->SetMass(-1.0f);
    model_col->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER | (u32)ComponentCollision::CollisionGroup::ETC);
    SetScaleAxisXYZ(float3{2.0f, 2.0f, 2.0f});
    return true;
}

void Bridge::Update() {}

void Bridge::Exit() {}

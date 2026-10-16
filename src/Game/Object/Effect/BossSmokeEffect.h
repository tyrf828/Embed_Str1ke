#pragma once
#include "EffectBase.h"

class BossSmokeEffect : public EffectBase
{
public:
    BP_OBJECT_DECL(BossSmokeEffect, "BossSmokeEffect");
    bool Init() override;
    void Update() override;
};

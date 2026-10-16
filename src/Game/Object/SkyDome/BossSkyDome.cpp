//===========================================================================
//! @file BossSkyDome.cpp
//! @brief スカイドーム
//===========================================================================
#include "BossSkyDome.h"
#include <System/Component/ComponentModel.h>

//! @brief 初期化
//! @return 初期化済み
bool BossSkyDome::Init()
{
    Super::Init();

    SetScaleAxisXYZ(float3{5.0f, 5.0f, 5.0f});

    auto model = AddComponent<ComponentModel>("data/Game/Model/Skydome/Space.mv1");

    model->RemoveZBuffer();
    return true;
}

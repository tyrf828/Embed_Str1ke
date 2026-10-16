//===========================================================================
//! @file BlueSkyDome.cpp
//! @brief スカイドーム
//===========================================================================
#include "BlueSkyDome.h"
#include <System/Component/ComponentModel.h>

//! @brief 初期化
//! @return 初期化済み
bool BlueSkyDome::Init()
{
    Super::Init();

    auto model = AddComponent<ComponentModel>("data/Game/Model/Skydome/Skydome.mv1")->SetScaleAxisXYZ(float3{3.5f, 3.5f, 3.5f});
    model->RemoveZBuffer();

    return true;
}

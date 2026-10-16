//===========================================================================
//! @file Sun.cpp
//! @brief 太陽
//===========================================================================
#include "Sun.h"
#include <System/Component/ComponentModel.h>

//! @brief 初期化
//! @return 初期化済み
bool Sun::Init()
{
    Super::Init();

    SetName("Sun");

    SetScaleAxisXYZ(float3{10.0f, 10.0f, 10.0f});
    auto model     = AddComponent<ComponentModel>("data/Game/Model/Sphere/Sphere.mv1");
    int mv1_handle = model->GetModel();
    MV1SetMaterialEmiColor(mv1_handle, 0, GetColorF(100.0f, 100.0f, 100.0f, 1.0f));

    return true;
}

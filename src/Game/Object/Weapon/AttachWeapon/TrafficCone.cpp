//===========================================================================
//! @file TrafficCone.cpp
//! @brief プレイヤーの武器
//===========================================================================
#include "TrafficCone.h"
#include <System/Component/ComponentModel.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
//! @return 初期化済み
bool TrafficCone::Init()
{
    SetName("TrafficCone");
    // モデルを設定
    AddComponent<ComponentModel>("data/Game/Model/AttachObject/Traffic_Cone/Traffic Cone.mv1")->SetScaleAxisXYZ(float3{0.15f, 0.15f, 0.15f});

    // コリジョンデータ設定
    collision_data_.radius_ = 4.5f;
    collision_data_.height_ = 13.0f;
    collision_data_.mass_   = 5.0f;

    AddComponent<Sound>()->Load({
        {"hit", "data/Game/SE/Weapon/軽いパンチ2.mp3", 256.0f, 0.7f},
    });

    return Super::Init();
}

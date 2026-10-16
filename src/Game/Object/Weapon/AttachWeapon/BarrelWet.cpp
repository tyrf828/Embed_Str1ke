//===========================================================================
//! @file BarrelWet.cpp
//! @brief プレイヤーの武器
//===========================================================================
#include "BarrelWet.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
//! @return 初期化済み
bool BarrelWet::Init()
{
    SetName("BarrelWet");

    // モデルを設定
    auto model = AddComponent<ComponentModel>("data/Game/Model/AttachObject/BarrelWet/Barrel Wet.mv1");
    model->SetScaleAxisXYZ(float3{0.13f, 0.13f, 0.13f});
    model->SetTranslate(float3{0.0f, 6.8f, 0.0f});
    // コリジョンデータ設定
    collision_data_.radius_ = 6.5f;
    collision_data_.height_ = 16.5f;
    collision_data_.mass_   = 10.0f;

    AddComponent<Sound>()->Load({
        {"hit", "data/Game/SE/Weapon/打撃6.mp3", 256.0f, 0.7f},
    });

    return Super::Init();
}

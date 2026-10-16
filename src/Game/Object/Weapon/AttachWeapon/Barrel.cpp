//===========================================================================
//! @file Barrel.cpp
//! @brief プレイヤーの武器
//===========================================================================
#include "Barrel.h"
#include <System/Component/ComponentModel.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
//! @return 初期化済み
bool Barrel::Init()
{
    SetName("Barrel");

    // モデルを設定
    auto model = AddComponent<ComponentModel>("data/Game/Model/AttachObject/Barrel/Barrel.mv1")
                     ->SetScaleAxisXYZ(float3{0.13f, 0.13f, 0.13f})
                     ->SetTranslate(float3{0.0f, 6.6f, 0.0f});

    // コリジョンデータ設定
    collision_data_.radius_ = 4.5f;
    collision_data_.height_ = 13.0f;
    collision_data_.mass_   = 5.0f;

    AddComponent<Sound>()->Load({
        {"hit", "data/Game/SE/Weapon/盾で防御.mp3", 256.0f, 0.7f},
    });

    return Super::Init();
}

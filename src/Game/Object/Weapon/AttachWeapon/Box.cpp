//===========================================================================
//! @file Box.cpp
//! @brief プレイヤーの武器
//===========================================================================
#include "Box.h"
#include <System/Component/ComponentModel.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
//! @return 初期化済み
bool Box::Init()
{
    SetName("Box");

    // モデルを設定
    AddComponent<ComponentModel>("data/Game/Model/AttachObject/Box/Metal_Box.mv1")
        ->SetScaleAxisXYZ(float3{0.2f, 0.2f, 0.2f})
        ->SetTranslate(float3{0.0f, 0.0f, 0.0f});

    // コリジョンデータ設定
    collision_data_.radius_ = 6.0f;
    collision_data_.height_ = 16.0f;
    collision_data_.mass_   = 10.0f;

    AddComponent<Sound>()->Load({
        {"hit", "data/Game/SE/Weapon/盾で防御.mp3", 256.0f, 0.7f},
    });

    return Super::Init();
}

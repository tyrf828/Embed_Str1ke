//===========================================================================
//! @file Truck.cpp
//! @brief トラック
//===========================================================================
#include "Truck.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
//! @return 初期化済み
bool Truck::Init()
{
    Super::Init();

    // モデルの追加
    AddComponent<ComponentModel>("data/Game/Model/Truck/ZIL130_NoCover.mv1");

    // プレイヤとの当たり判定
    auto model_col = AddComponent<ComponentCollisionModel>();
    model_col->AttachToModel();
    model_col->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER);

    // ステージとの当たり判定
    auto col = AddComponent<ComponentCollisionCapsule>();
    col->SetRadius(1.5f);
    col->SetHeight(10.0f);
    col->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::GROUND);
    col->UseGravity(true);

    // サウンドの追加
    AddComponent<Sound>()->Load({
        {"truck_stop",  "data/Game/SE/ETC/車の急ブレーキ1.mp3", 512.0f, 0.8f},
        {"truck_start", "data/Game/SE/ETC/車通過1.mp3",         512.0f, 1.0f},
    });

    SetScaleAxisXYZ(float3{1.3f, 1.3f, 1.3f});

    return true;
}

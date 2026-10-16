//===========================================================================
//! @file Bottle.cpp
//! @brief 回復アイテム
//===========================================================================
#include "Bottle.h"
#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentModel.h>

//! @brief 初期化
//! @return 初期化済み
bool Bottle::Init()
{
    Super::Init();
    // モデルを設定
    AddComponent<ComponentModel>("data/Game/Model/Bottle/Generate_a_mystical_M_1224102412_texture.mv1")
        ->SetScaleAxisXYZ(float3{0.15f, 0.15f, 0.15f})
        ->SetRotationAxisXYZ(float3{0.0f, 0.0f, 89.0f});

    // プレイヤーの左手に付ける
    AddComponent<ComponentAttachModel>()->SetAttachObject("Player", "mixamorig:LeftHand");
    auto attach_model = GetComponent<ComponentAttachModel>();
    attach_model->SetAttachOffset(float3{10.0f, 10.0f, -5.0f});

    return true;
}

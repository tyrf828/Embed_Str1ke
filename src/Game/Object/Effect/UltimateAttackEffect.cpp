//===========================================================================
//! @file UltimateAttaclEffect.cpp
//! @brief 必殺技エフェクト
//===========================================================================
#include "UltimateAttackEffect.h"
#include <System/Component/ComponentEffect.h>
#include <Game/Object/Player/Player.h>

//! @brief 初期化
//! @return 初期化済み
bool UltimateAttackEffect::Init()
{
    Super::Init();
    // エフェクトコンポーネントを追加
    auto effect = AddComponent<ComponentEffect>("data/Game/Effect/Ult/bullet.efkefc");

    SetScaleAxisXYZ({1.5f, 1.5f, 1.5f});   // サイズを小さくする
    if(auto player = Scene::Object::Get<Player>())
    {
        // プレイヤーの位置に合わせる
        SetTranslate(player->GetTranslate());
        init_height_ = GetTranslate().y;
    }

    Play();

    return true;
}

//! @brief 更新
void UltimateAttackEffect::Update()
{
    Super::Update();
    auto effect = GetComponent<ComponentEffect>();
    if(GetTranslate().y <= init_height_ + HEIGHT_)
    {
        AddTranslate(-GetVectorAxisZ() * SPEED_);
    }

    if(!effect->IsPlaying())
    {
        Play();
    }
}

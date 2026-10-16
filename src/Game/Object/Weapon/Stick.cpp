//===========================================================================
//! @file Stick.cpp
//! @brief プレイヤーの武器
//===========================================================================
#include "Stick.h"
#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/WeaponAttack.h>
#include <Game/Object/Effect/HitEffect.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/Sound.h>
#include <System/Component/ComponentModel.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Component/Weapon/AttachAttack.h>
#include <Game/Component/Weapon/AttachWeaponParts.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyAction.h>
#include <Game/Object/Enemy/EnemyBase.h>

//! @brief 初期化
//! @return 初期化済み
bool Stick::Init()
{
    Super::Init();

    SetName("Stick");

    SetScaleAxisXYZ(float3{1.2f, 1.2f, 1.2f});

    // モデルを設定
    AddComponent<ComponentModel>("data/Game/Model/Wood Stick/woodstick.mv1")
        ->SetScaleAxisXYZ(float3{0.25f, 0.25f, 0.25f})
        ->SetRotationAxisXYZ(float3{0.0f, 0.0f, 0.0f});

    // プレイヤーの右手に付ける
    AddComponent<ComponentAttachModel>()->SetAttachObject("Player", "mixamorig:RightHand");
    auto attach_model = GetComponent<ComponentAttachModel>();
    attach_model->SetAttachRotate(float3{0.0f, 30.0f, 0.0f});
    attach_model->SetAttachOffset(float3{21.0f, 20.0f, -1.2f});

    // 衝突判定のコンポーネントを追加
    auto col = AddComponent<ComponentCollisionCapsule>()
                   ->SetTranslate({-16.0f, -11.0f, -14.0f})
                   ->SetRotationAxisXYZ({55.0f, 0.0f, -41.0f})
                   ->SetRadius(10.0f)
                   ->SetHeight(100.0f)
                   ->SetCollisionGroup(ComponentCollision::CollisionGroup::ETC)
                   ->SetOverlapCollisionGroup((u32)ComponentCollision::CollisionGroup::ENEMY | (u32)ComponentCollision::CollisionGroup::GROUND |
                                              (u32)ComponentCollision::CollisionGroup::WEAPON | (u32)ComponentCollision::CollisionGroup::ETC |
                                              (u32)ComponentCollision::CollisionGroup::ITEM);
    // 武器攻撃のコンポーネントを追加
    AddComponent<WeaponAttack>();

    AddComponent<AttachWeaponParts>();
    // 読み込み
    AddComponent<Sound>()->Load({
        {"hit", "data/Game/SE/Weapon/軽いパンチ2.mp3", 128.0f, 0.7f},
    });

    return true;
}
//! @brief 更新
void Stick::Update()
{
    Super::Update();
}

//! @brief 衝突時の処理
//! @param hit_info 衝突したオブジェクトの情報
void Stick::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    auto col = GetComponent<ComponentCollisionCapsule>();
    if(!col)
    {
        return;
    }
    // 衝突情報を渡す
    if(auto attach_attack = GetComponent<AttachAttack>())
    {
        attach_attack->OnHit(hit_info);
    }

    // 敵に当たったら音を再生
    if(static_cast<u32>(hit_info.hit_collision_->GetCollisionGroup()) & static_cast<u32>(ComponentCollision::CollisionGroup::ENEMY))
    {
        if(auto se = GetComponent<Sound>())
        {
            se->PlayNoSame("hit");
        }
    }
    Super::OnHit(hit_info);
}

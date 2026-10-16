//===========================================================================
//! @file GunHitCheck.cpp
//! @brief 敵の武器の射撃判定
//===========================================================================
#include "GunHitCheck.h"
#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/Enemy/AttackParam.h>
#include <Game/Object/Effect/ShootLaserEffect.h>
#include <Game/Object/Effect/EnemyHitEffect.h>

//! @brief 初期化
//! @return 初期化済み
bool GunHitCheck::Init()
{
    Super::Init();

    SetName("GunHitCheck");

    auto col = AddComponent<ComponentCollisionCapsule>();
    col->SetRotationAxisXYZ(float3{90.0f, 0.0f, 0.0f});
    col->SetRadius(2.0f);
    col->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY_WEAPON);
    col->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER);
    col->SetOverlapCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER);

    AddComponent<AttackParam>();
    return true;
}
//! @brief 更新
void GunHitCheck::Update()
{
    Super::Update();
    if(owner_.lock())
    {
        if(auto attack = GetComponent<AttackParam>())
        {
            // オーナーの攻撃力から取得
            if(auto owner_attack = owner_.lock()->GetComponent<AttackParam>())
            {
                attack->SetAttack(owner_attack->GetAttack());
            }
        }
    }
}

//! @brief 衝突時の処理
//! @param hit_info 衝突したオブジェクトの情報
void GunHitCheck::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    if(static_cast<u32>(hit_info.hit_collision_->GetCollisionGroup()) & static_cast<u32>(ComponentCollision::CollisionGroup::PLAYER))
    {
        Scene::Object::Create<EnemyHitEffect>()->SetTranslate(hit_info.hit_position_)->SetScaleAxisXYZ(float3{2.0f, 2.0f, 2.0f});
        Scene::Object::Release(shared_from_this());
    }

    Super::OnHit(hit_info);
}

//! @brief 終了
void GunHitCheck::Exit()
{
    Super::Exit();
}

//! @brief 攻撃距離を設定
//! @param length 距離
void GunHitCheck::SetAttackLength(float length)
{
    if(auto col = GetComponent<ComponentCollisionCapsule>())
    {
        col->SetHeight(length);
    }
}

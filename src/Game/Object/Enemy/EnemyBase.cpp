//===========================================================================
//! @file EnemyBase.cpp
//! @brief 敵の基底クラス
//===========================================================================
#include "EnemyBase.h"
#include <Game/Object/Player/Player.h>
#include <System/Component/ComponentModel.h>
#include <Game/Object/Manager/EnemyMentalManager.h>

//! @brief 更新
void EnemyBase::Update()
{
    Super::Update();
    if(!Scene::Object::Get<Player>())
    {
        return;
    }
    if(auto p_model = Scene::Object::Get<Player>()->GetComponent<ComponentModel>())
    {
        // ヒットストップ中でなければ無敵時間を減少
        if(!p_model->IsHitStopping())
        {
            if(invincible_time_ > 0.0f)
            {
                invincible_time_ -= GetDeltaTime();
                invincible_time_  = std::max(0.0f, invincible_time_);
            }
        }
    }
}

//! @brief 無敵かどうか
//! @retval true 無敵
//! @retval false 無敵ではない
bool EnemyBase::IsInvincible() const
{
    return invincible_time_ > 0.0f;
}

//! @brief コリジョンをリセット
void EnemyBase::ResetCollision()
{
    {
        auto col = GetComponents<ComponentCollisionCapsule>();
        for(auto& c : col)
        {
            RemoveComponent(c);
        }
    }
    auto col = AddComponent<ComponentCollisionCapsule>();
    col->SetRadius(collision_info_.radius_)
        ->SetHeight(collision_info_.height_)
        ->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY)
        ->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::GROUND | (u32)ComponentCollision::CollisionGroup::PLAYER |
                               (u32)ComponentCollision::CollisionGroup::WEAPON | (u32)ComponentCollision::CollisionGroup::DODGE |
                               (u32)ComponentCollision::CollisionGroup::ENEMY | (u32)ComponentCollision::CollisionGroup::ULTIMATE_ATTACK |
                               (u32)ComponentCollision::CollisionGroup::ITEM)
        ->SetMass(collision_info_.mass_)
        ->UseGravity();
}

//! @brief オブジェクト情報をリセット
void EnemyBase::ResetObjectInfo()
{
    ResetCollision();
}

//===========================================================================
//! @file UltimateAttackCollision.cpp
//! @brief 必殺技の衝突判定
//===========================================================================
#include "UltimateAttackCollision.h"
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyAction.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyKnockback.h>
#include <Game/Component/Enemy/Knockback.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Object/Effect/DamageText.h>
#include <Game/Object/Enemy/EnemyBase.h>

//! @brief 初期化
//! @return 初期化済み
bool UltimateAttackCollision::Init()
{
    Super::Init();

    AddComponent<ComponentCollisionSphere>()
        ->SetCollisionGroup(ComponentCollision::CollisionGroup::ULTIMATE_ATTACK)
        ->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::ENEMY)
        ->SetOverlapCollisionGroup((u32)ComponentCollision::CollisionGroup::ENEMY);

    return true;
}

//! @brief 更新処理
void UltimateAttackCollision::Update()
{
    Super::Update();
}

//! @brief 衝突時の処理
void UltimateAttackCollision::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    auto other = hit_info.hit_collision_->GetCollisionGroup();
    // 敵と衝突した
    if(other == ComponentCollision::CollisionGroup::ENEMY)
    {
        // 敵を空を飛ばせる
        auto enemy           = hit_info.hit_collision_->GetOwner();
        auto enemy_collision = enemy->GetComponent<ComponentCollisionCapsule>();

        enemy->AddTranslate({0.0f, 0.1f, 0.0f}, true);
        enemy_collision->UseGravity(false);   // 重力を無効化

        if(!enemy_vec_.contains(enemy))
        {
            enemy_vec_.insert(enemy);
        }
        // ダメージ表示
        auto text       = Scene::Object::Create<DamageText>();
        float3 rand     = {GetRand(5), GetRand(15), GetRand(5)};
        int rand_damage = 100 + GetRand(400);
        text->Set(rand_damage, enemy->GetTranslate() + float3(rand));
    }

    Super::OnHit(hit_info);
}

//! @brief 終了処理
void UltimateAttackCollision::Exit()
{
    Super::Exit();

    auto player_pos = Scene::Object::Get<Player>()->GetTranslate();

    for(auto enemy : enemy_vec_)
    {
        if(auto enemy_collision = enemy->GetComponent<ComponentCollisionCapsule>())
        {
            enemy_collision->UseGravity(true);   // 重力を有効化
        }
        float3 direction = enemy->GetTranslate() - player_pos;
        direction.y      = abs(direction.y);
        if(auto hp = enemy->GetComponent<ComponentHP>())
        {
            // 敵のHPを削る
            hp->SubHP(999);
            auto text = Scene::Object::Create<DamageText>();
            text->Set(999, enemy->GetTranslate() + float3(0.0f, 1.0f, 0.0f));
        }
        if(auto enemy_action = enemy->GetComponent<EnemyActionBase>())
        {
            // ノックバックアクションに遷移
            enemy_action->SetKnockback();

            // ノックバック方向を設定
            if(auto knockback = enemy->GetComponent<Knockback>())
            {
                knockback->SetDirection(direction, 2.0f);
            }
        }
    }

    // 敵のポインタをクリア
    enemy_vec_.clear();
}

//! @brief 攻撃範囲の設定
void UltimateAttackCollision::SetAttackRadius(float radius)
{
    // 攻撃範囲の設定
    if(auto collision = GetComponent<ComponentCollisionSphere>())
    {
        collision->SetRadius(radius);
    }
}

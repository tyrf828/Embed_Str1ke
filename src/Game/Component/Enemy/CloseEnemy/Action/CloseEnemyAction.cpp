//===========================================================================
//! @file CloseEnemyAction.cpp
//! @brief 敵の行動コンポーネント
//===========================================================================
#include "CloseEnemyAction.h"
#include "System/Component/ComponentModel.h"
#include <Game/Object/Player/Player.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyAttack.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyIdle.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyDamaged.h>
#include <Game/Object/Enemy/EnemyBase.h>
#include <Game/Component/WeaponAttack.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyFindPlayer.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyKnockback.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyStayby.h>
#include <Game/Object/Weapon/Stick.h>
#include <Game/Component/Enemy/Knockback.h>

//! @brief 初期化
void CloseEnemyAction::Init()
{
    Super::Init();
    SetOwner(GetOwnerPtr());
    SetAction(std::make_shared<CloseEnemyStayby>());
}

//! @brief 更新
void CloseEnemyAction::Update()
{
    Super::Update();
    now_action_->Update();

    // 敵のノックバックするカウンターを少しずつ減らす
    {
        const float RESET_HIT_TIME = 3.0f;
        if(hit_timer_ >= RESET_HIT_TIME)
        {
            hit_timer_ = 0.0f;
            hit_count_--;
            hit_count_ = std::max(0, hit_count_);
        }
        else if(hit_count_ > 0)
        {
            hit_timer_ += GetDeltaTime();
        }
    }
}

//! @brief 更新後処理
void CloseEnemyAction::PostUpdate()
{
    Super::PostUpdate();
    now_action_->PostUpdate();
}

//! @brief 遅延描画
void CloseEnemyAction::LateDraw()
{
    Super::LateDraw();
    now_action_->LateDraw();
}

//! @brief GUI表示処理
void CloseEnemyAction::GUI()
{
    Super::GUI();
    now_action_->GUI();
}

//! @brief 衝突時の処理
void CloseEnemyAction::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    auto other = hit_info.hit_collision_->GetCollisionGroup();

    // 必殺技を受けた場合は直接被ダメージアクションに移行
    if(static_cast<u32>(other) & static_cast<u32>(ComponentCollision::CollisionGroup::ULTIMATE_ATTACK))
    {
        SetAction(std::make_shared<CloseEnemyDamaged>());
    }
    // 攻撃を受けた
    if(hit_info.hit_collision_->GetCollisionGroup() != ComponentCollision::CollisionGroup::WEAPON)
    {
        return;
    }

    auto base = std::dynamic_pointer_cast<EnemyBase>(GetOwnerPtr());
    if(base == nullptr)
    {
        return;
    }
    // 無敵中は無視
    if(base->IsInvincible())
    {
        return;
    }

    // 攻撃処理
    if(auto weapon_atk = hit_info.hit_collision_->GetOwner()->GetComponent<WeaponAttack>())
    {
        weapon_atk->HitEnemy(hit_info);
    }

    if(auto hp = GetOwner()->GetComponent<ComponentHP>())
    {
        auto weapon = Scene::Object::Get<Stick>();
        // HPを減らす
        if(auto wea_atk = weapon->GetComponent<WeaponAttack>())
        {
            hp->SubHP(wea_atk->GetATK());
        }
    }
    const int HIT_KNOCKBACK = 3;
    hit_count_++;
    hit_timer_ = 0.0f;
    if(hit_count_ < HIT_KNOCKBACK)
    {
        // 被ダメージアクションに移行
        SetAction(std::make_shared<CloseEnemyDamaged>());
        // ヒットストップ
        if(auto model = GetOwner()->GetComponent<ComponentModel>())
        {
            model->SetHitStop(0.2f);
        }
    }
    else
    {
        // プレイヤーが立ち留まったまま連打する作業感を減らすため、近距離の敵のみノックバックする
        SetKnockback();
        // ノックバック方向を設定
        if(auto knockback = GetOwner()->GetComponent<Knockback>())
        {
            knockback->SetDirection(GetOwner()->GetVectorAxisZ() * 0.1f + float3{0.0f, 0.05f, 0.0f}, 1.1f);
        }
        hit_count_ = 0;
    }

    // 無敵時間セット
    base->SetInvincibleTime(0.1f);
}

//! @brief 前のアクションを取得
//! @return 前のアクション
std::shared_ptr<EnemyActionBase> CloseEnemyAction::GetPrevAction() const
{
    if(prev_action_ == nullptr)
    {
        return std::make_shared<CloseEnemyIdle>();
    }
    return prev_action_;
}

//! @brief 攻撃アクションを設定
void CloseEnemyAction::SetAttack()
{
    SetAction(std::make_shared<CloseEnemyAttack>());
}

//! @brief ノックバックアクションを設定
void CloseEnemyAction::SetKnockback()
{
    SetAction(std::make_shared<CloseEnemyKnockback>());
}

//! @brief プレイヤーを探すアクションを設定
void CloseEnemyAction::SetFindPlayer()
{
    SetAction(std::make_shared<CloseEnemyFindPlayer>());
}

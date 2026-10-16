//===========================================================================
//! @file FarEnemyAction.cpp
//! @brief 敵の行動コンポーネント
//===========================================================================
#include "FarEnemyAction.h"
#include "System/Component/ComponentModel.h"
#include <Game/Object/Player/Player.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Object/Enemy/EnemyBase.h>
#include <Game/Component/WeaponAttack.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyAttack.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyIdle.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyDamaged.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyDeath.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyThreat.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyFindPlayer.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyKnockback.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyStayby.h>
#include <Game/Object/Weapon/Stick.h>

//! @brief 初期化
void FarEnemyAction::Init()
{
    Super::Init();
    SetOwner(GetOwnerPtr());
    SetAction(std::make_shared<FarEnemyStayby>());
}

//! @brief 更新
void FarEnemyAction::Update()
{
    Super::Update();
    now_action_->Update();
}

//! @brief 更新後処理
void FarEnemyAction::PostUpdate()
{
    Super::PostUpdate();
    now_action_->PostUpdate();
}

void FarEnemyAction::LateDraw()
{
    Super::LateDraw();
    now_action_->LateDraw();
}

//! @brief GUI表示処理
void FarEnemyAction::GUI()
{
    Super::GUI();
    now_action_->GUI();
}

//! @brief 衝突時の処理
//! @param hit_info 衝突したオブジェクトの情報
void FarEnemyAction::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    auto other = hit_info.hit_collision_->GetCollisionGroup();
    // 必殺技を受けた場合は直接被ダメージアクションに移行
    if(other == ComponentCollision::CollisionGroup::ULTIMATE_ATTACK)
    {
        SetAction(std::make_shared<FarEnemyDamaged>());
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

    // ヒットストップ
    if(auto model = GetOwner()->GetComponent<ComponentModel>())
    {
        model->SetHitStop(0.2f);
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

    // 被ダメージアクションに移行
    SetAction(std::make_shared<FarEnemyDamaged>());

    // 無敵時間セット
    base->SetInvincibleTime(0.1f);
}

//! @brief 前のアクションを取得
//! @return 前のアクション
std::shared_ptr<EnemyActionBase> FarEnemyAction::GetPrevAction() const
{
    if(prev_action_ == nullptr)
    {
        return std::make_shared<FarEnemyIdle>();
    }
    return prev_action_;
}

//! @brief 攻撃アクションを設定
void FarEnemyAction::SetAttack()
{
    SetAction(std::make_shared<FarEnemyAttack>());
}

//! @brief ノックバックアクションを設定
void FarEnemyAction::SetKnockback()
{
    SetAction(std::make_shared<FarEnemyKnockback>());
}

//! @brief プレイヤーを探すアクションを設定
void FarEnemyAction::SetFindPlayer()
{
    SetAction(std::make_shared<FarEnemyFindPlayer>());
}

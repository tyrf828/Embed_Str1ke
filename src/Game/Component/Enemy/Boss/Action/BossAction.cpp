//===========================================================================
//! @file BossAction.cpp
//! @brief ボスの行動コンポーネント
//===========================================================================
#include "BossAction.h"
#include "System/Component/ComponentModel.h"
#include <Game/Object/Player/Player.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Object/Enemy/EnemyBase.h>
#include <Game/Component/WeaponAttack.h>
#include <Game/Object/Weapon/Stick.h>
#include <Game/Component/Enemy/Boss/Action/BossIdle.h>
#include <Game/Component/Enemy/Boss/Action/BossDashAttack.h>
#include <Game/Component/Enemy/Boss/Action/BossKick.h>
#include <Game/Component/Enemy/Boss/Action/BossJump.h>
#include <Game/Component/Enemy/Boss/Action/BossFallBack.h>
#include <Game/Component/Enemy/Boss/Action/BossGrabAttack.h>
#include <Game/Component/Enemy/Boss/Action/BossConfront.h>
#include <Game/Component/Enemy/Boss/Action/BossBlast.h>
#include <Game/Component/Enemy/Boss/Action/BossDamaged.h>
#include <Game/Component/Enemy/Boss/Action/BossDeath.h>
#include <Game/Component/Enemy/Boss/Action/BossKnockback.h>
#include <Game/Component/Enemy/Boss/Action/BossWalk.h>
#include <Game/Component/Enemy/Boss/Action/BossReady.h>
#include <Game/Component/Enemy/Boss/BossActionManager.h>

//! @brief 初期化
void BossAction::Init()
{
    Super::Init();
    SetOwner(GetOwnerPtr());
    SetAction(std::make_shared<BossReady>());
}

//! @brief 更新
void BossAction::Update()
{
    Super::Update();
    auto action = now_action_;
    if(action)
    {
        action->Update();
    }
}

//! @brief 遅延描画
void BossAction::LateDraw()
{
    Super::LateDraw();
    now_action_->LateDraw();
}

//! @brief 更新後処理
void BossAction::PostUpdate()
{
    Super::PostUpdate();
    now_action_->PostUpdate();
}

//! @brief GUI表示処理
void BossAction::GUI()
{
    Super::GUI();
    now_action_->GUI();
}

//! @brief 衝突時の処理
//! @param hit_info 衝突したオブジェクトの情報
void BossAction::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    now_action_->OnHit(hit_info);

    auto other          = hit_info.hit_collision_->GetCollisionGroup();
    auto current_action = GetCurrentAction();
    bool enable_damaged = false;

    // ダメージを受けられるアクションか判定
    if(dynamic_cast<BossIdle*>(current_action.get()) || dynamic_cast<BossWalk*>(current_action.get()) || dynamic_cast<BossConfront*>(current_action.get()) ||
       dynamic_cast<BossDamaged*>(current_action.get()))
    {
        enable_damaged = true;
    }

    // 必殺技を受けた場合
    if(other == ComponentCollision::CollisionGroup::ULTIMATE_ATTACK)
    {
        SetAction(std::make_shared<BossDamaged>());
        return;
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

    // 武器攻撃のヒット処理
    if(auto weapon_atk = hit_info.hit_collision_->GetOwner()->GetComponent<WeaponAttack>())
    {
        weapon_atk->HitEnemy(hit_info);
    }

    // ヒットストップ
    if(auto model = GetOwner()->GetComponent<ComponentModel>())
    {
        model->SetHitStop(0.2f);
    }

    // HP減少処理
    if(auto hp = GetOwner()->GetComponent<ComponentHP>())
    {
        auto weapon = Scene::Object::Get<Stick>();
        // HPを減らす
        if(auto wea_atk = weapon->GetComponent<WeaponAttack>())
        {
            hp->SubHP(wea_atk->GetATK());

            if(auto manager = GetOwner()->GetComponent<BossActionManager>())
            {
                manager->AddHitNumCount();
            }

            // HPが0以下なら死亡
            if(hp->GetHP() <= 0)
            {
                SetAction(std::make_shared<BossDeath>());
                base->SetInvincibleTime(0.1f);
                return;
            }
        }
    }

    // ダメージアクションへの遷移
    if(enable_damaged)
    {
        SetAction(std::make_shared<BossDamaged>());
    }

    // 無敵時間設定
    base->SetInvincibleTime(0.1f);
}

//! @brief 前のアクションを取得
//! @return 前のアクション
std::shared_ptr<EnemyActionBase> BossAction::GetPrevAction() const
{
    if(prev_action_ == nullptr)
    {
        return std::make_shared<BossIdle>();
    }
    return prev_action_;
}

//! @brief ノックバックを設定
void BossAction::SetKnockback()
{
    SetAction(std::make_shared<BossKnockback>());
}

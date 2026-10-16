//===========================================================================
//! @file CloseEnemyDeath.cpp
//! @brief 敵死亡コンポーネント
//===========================================================================
#include "CloseEnemyDeath.h"
#include <Game/Component/Enemy/CloseEnemy/CloseEnemyAnimationController.h>
#include <Game/Component/Sound.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/WeaponAttack.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Object/Weapon/Stick.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyAction.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyStayby.h>
#include <Game/Object/Enemy/EnemyBase.h>
#include <Game/Component/DissolveEffect.h>
#include <Game/Object/Enemy/CloseEnemy/CloseEnemy.h>
#include <Game/Object/Manager/EnemyMentalManager.h>

//! @brief 初期化
void CloseEnemyDeath::Init()
{
    Super::Init();

    if(auto anim_control = GetOwner()->GetComponent<CloseEnemyAnimationController>())
    {
        // 死亡アニメーションを設定
        anim_control->ResetAllFlags();
        anim_control->SetFlag(CloseEnemyAnimationController::STATE_BIT::DEATH);
    }
    if(auto hp = GetOwner()->GetComponent<ComponentHP>())
    {
        hp->SetHP(0);
    }
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 速度をリセット
        move->SetVelocity(float3{0, 0, 0});
    }
    if(auto col = GetOwner()->GetComponent<ComponentCollisionCapsule>())
    {
        // 衝突グループを地面に変更
        col->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::GROUND);
    }
    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        // 音を再生
        se->PlayNoSame("death");
    }

    if(auto mental_manager = Scene::Object::Get<EnemyMentalManager>())
    {
        // 登録解除
        mental_manager->ReleaseEnemyList(std::dynamic_pointer_cast<EnemyBase>(GetOwner()));
    }
}

//! @brief 更新
void CloseEnemyDeath::Update()
{
    Super::Update();
    // ロックポイントを解放
    if(auto owner = dynamic_cast<EnemyBase*>(GetOwner().get()))
    {
        owner->ClearEnemyLockPoint();
    }
    if(auto anim_control = GetOwner()->GetComponent<CloseEnemyAnimationController>())
    {
        // アニメーションを再生中なら待機
        if(anim_control->IsPlaying())
        {
            return;
        }
        // ディゾルブエフェクト開始
        if(auto weapon = dynamic_cast<CloseEnemy*>(GetOwner().get())->GetWeaponPtr())
        {
            auto weapon_dissolve = weapon->GetComponent<DissolveEffect>();
            if(!weapon_dissolve)
            {
                weapon->AddComponent<DissolveEffect>()->StartDissolve();
            }
        }
        // ディゾルブエフェクト開始
        auto dissolve = GetOwner()->GetComponent<DissolveEffect>();
        if(!dissolve)
        {
            dissolve = GetOwner()->AddComponent<DissolveEffect>();
            dissolve->StartDissolve();
        }
        else
        {
            // ディゾルブ完了後、待機状態へ遷移
            if(dissolve->GetDissolveAmount() >= 1.0f)
            {
                if(auto e_action = GetOwner()->GetComponent<CloseEnemyAction>())
                {
                    e_action->SetAction(std::make_shared<CloseEnemyStayby>());
                }
            }
        }
    }
}
//! @brief 終了
void CloseEnemyDeath::Exit()
{
    Super::Exit();
    if(GetOwner()->GetComponent<ComponentCollisionCapsule>())
    {
        GetOwner()->RemoveComponent<ComponentCollisionCapsule>();
    }
    if(auto anim_control = GetOwner()->GetComponent<CloseEnemyAnimationController>())
    {
        anim_control->ResetFlag(CloseEnemyAnimationController::STATE_BIT::DEATH);
    }
}

//! @brief GUI表示処理
void CloseEnemyDeath::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("EnemyDeath"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//===========================================================================
//! @file FarEnemyDeath.cpp
//! @brief 敵死亡コンポーネント
//===========================================================================
#include "FarEnemyDeath.h"
#include <System/Component/ComponentModel.h>
#include <Game/Component/Enemy/FarEnemy/FarEnemyAnimationController.h>
#include <Game/Component/Sound.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/WeaponAttack.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Object/Weapon/Stick.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyAction.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyStayby.h>
#include <Game/Object/Enemy/EnemyBase.h>
#include <Game/Component/DissolveEffect.h>
#include <Game/Object/Enemy/FarEnemy/Farenemy.h>
#include <Game/Object/Manager/EnemyMentalManager.h>

//! @brief 初期化
void FarEnemyDeath::Init()
{
    Super::Init();

    if(auto anim_control = GetOwner()->GetComponent<FarEnemyAnimationController>())
    {
        // 死亡アニメーションを設定
        anim_control->SetFlag(FarEnemyAnimationController::STATE_BIT::DEATH);
    }
    if(auto hp = GetOwner()->GetComponent<ComponentHP>())
    {
        // HPを0に固定
        hp->SetHP(0);
    }
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 速度をリセット
        move->SetVelocity(float3{0, 0, 0});
    }
    if(auto col = GetOwner()->GetComponent<ComponentCollisionCapsule>())
    {
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
void FarEnemyDeath::Update()
{
    Super::Update();
    // ロックポイントを解放
    if(auto owner = dynamic_cast<EnemyBase*>(GetOwner().get()))
    {
        owner->ClearEnemyLockPoint();
    }

    if(auto anim_control = GetOwner()->GetComponent<FarEnemyAnimationController>())
    {
        // アニメーションを再生中なら待機
        if(anim_control->IsPlaying())
        {
            return;
        }
        // ディゾルブエフェクト開始
        if(auto weapon = dynamic_cast<FarEnemy*>(GetOwner().get())->GetWeaponPtr())
        {
            if(!weapon->GetComponent<DissolveEffect>())
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
                // 待機状態へ遷移
                if(auto e_action = GetOwner()->GetComponent<FarEnemyAction>())
                {
                    e_action->SetAction(std::make_shared<FarEnemyStayby>());
                }
            }
        }
    }
}
//! @brief 終了
void FarEnemyDeath::Exit()
{
    Super::Exit();
    if(GetOwner()->GetComponent<ComponentCollisionCapsule>())
    {
        GetOwner()->RemoveComponent<ComponentCollisionCapsule>();
    }
    if(auto anim_control = GetOwner()->GetComponent<FarEnemyAnimationController>())
    {
        anim_control->ResetFlag(FarEnemyAnimationController::STATE_BIT::DEATH);
    }
}

//! @brief GUI表示処理
void FarEnemyDeath::GUI()
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

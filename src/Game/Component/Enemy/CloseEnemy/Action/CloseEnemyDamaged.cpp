//===========================================================================
//! @file CloseEnemyDamaged.cpp
//! @brief 敵がダメージ受けたコンポーネント
//===========================================================================
#include "CloseEnemyDamaged.h"
#include <Game/Component/Enemy/CloseEnemy/CloseEnemyAnimationController.h>
#include <Game/Component/Sound.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/WeaponAttack.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Object/Weapon/Stick.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyAction.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyDeath.h>

//! @brief 初期化
void CloseEnemyDamaged::Init()
{
    Super::Init();
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動停止
        move->SetMove();
        move->SetVelocity(float3{0, 0, 0});
    }
    if(auto anim_control = GetOwner()->GetComponent<CloseEnemyAnimationController>())
    {
        // アニメーション再生
        if(!anim_control->GetFlag(CloseEnemyAnimationController::STATE_BIT::HIT))
        {
            anim_control->SetFlag(CloseEnemyAnimationController::STATE_BIT::HIT);
        }
        else
        {
            anim_control->PlayAgain();
        }
    }
    if(auto hp = GetOwner()->GetComponent<ComponentHP>())
    {
        // HPが0以下なら死亡アクションへ遷移
        if(hp->GetHP() <= 0)
        {
            if(auto e_action = GetOwner()->GetComponent<CloseEnemyAction>())
            {
                e_action->SetAction(std::make_shared<CloseEnemyDeath>());
                return;
            }
        }
    }
    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        // 音を再生
        se->PlayNoSame("hit");
    }
}

//! @brief 更新
void CloseEnemyDamaged::Update()
{
    Super::Update();
    if(auto anim_control = GetOwner()->GetComponent<CloseEnemyAnimationController>())
    {
        // アニメーション終了後、前のアクションに戻る
        if(!anim_control->GetFlag(CloseEnemyAnimationController::STATE_BIT::HIT))
        {
            if(auto e_action = GetOwner()->GetComponent<CloseEnemyAction>())
            {
                e_action->SetAction(e_action->GetPrevAction());
            }
        }
    }
}
//! @brief 終了
void CloseEnemyDamaged::Exit()
{
    Super::Exit();
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動再開
        move->SetMove(true);
    }
}

//! @brief GUI表示処理
void CloseEnemyDamaged::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("EnemyDamaged"))
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

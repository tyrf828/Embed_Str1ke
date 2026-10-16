//===========================================================================
//! @file FarEnemyDamaged.cpp
//! @brief 敵がダメージ受けたコンポーネント
//===========================================================================
#include "FarEnemyDamaged.h"
#include <Game/Component/Enemy/FarEnemy/FarEnemyAnimationController.h>
#include <Game/Component/Sound.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/WeaponAttack.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Object/Weapon/Stick.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyAction.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyDeath.h>

//! @brief 初期化
void FarEnemyDamaged::Init()
{
    Super::Init();
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動停止
        move->SetMove();
    }
    if(auto anim_control = GetOwner()->GetComponent<FarEnemyAnimationController>())
    {
        // アニメーション再生
        if(!anim_control->GetFlag(FarEnemyAnimationController::STATE_BIT::HIT))
        {
            anim_control->SetFlag(FarEnemyAnimationController::STATE_BIT::HIT);
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
            if(auto e_action = GetOwner()->GetComponent<FarEnemyAction>())
            {
                e_action->SetAction(std::make_shared<FarEnemyDeath>());
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
void FarEnemyDamaged::Update()
{
    Super::Update();
    if(auto anim_control = GetOwner()->GetComponent<FarEnemyAnimationController>())
    {
        // アニメーション終了後、前のアクションに戻る
        if(!anim_control->GetFlag(FarEnemyAnimationController::STATE_BIT::HIT))
        {
            if(auto e_action = GetOwner()->GetComponent<FarEnemyAction>())
            {
                e_action->SetAction(e_action->GetPrevAction());
            }
        }
    }
}
//! @brief 終了
void FarEnemyDamaged::Exit()
{
    Super::Exit();

    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動再開
        move->SetMove(true);
    }
}

//! @brief GUI表示処理
void FarEnemyDamaged::GUI()
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

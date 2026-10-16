//===========================================================================
//! @file CloseEnemyKnockback.cpp
//! @brief 敵のノックバックコンポーネント
//===========================================================================
#include "CloseEnemyKnockback.h"
#include <Game/Component/Enemy/CloseEnemy/CloseEnemyAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/Enemy/Knockback.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyAction.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyStandUp.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyDeath.h>

//! @brief 初期化
void CloseEnemyKnockback::Init()
{
    Super::Init();
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動停止
        move->SetMove();
    }
    if(auto anim_control = GetOwner()->GetComponent<CloseEnemyAnimationController>())
    {
        // ノックバックアニメーションを設定
        anim_control->SetFlag(CloseEnemyAnimationController::STATE_BIT::BIG_HIT);
        anim_control->ResetFlag(CloseEnemyAnimationController::STATE_BIT::HIT);
    }
    // ノックバック処理を追加
    GetOwner()->AddComponent<Knockback>();

    // HPが0以下なら死亡アクションへ遷移
    if(GetOwner()->GetComponent<ComponentHP>()->GetHP() <= 0)
    {
        if(auto e_action = GetOwner()->GetComponent<CloseEnemyAction>())
        {
            e_action->SetAction(std::make_shared<CloseEnemyDeath>());
        }
    }
}

//! @brief 更新
void CloseEnemyKnockback::Update()
{
    Super::Update();
    auto anim_control = GetOwner()->GetComponent<CloseEnemyAnimationController>();
    if(!anim_control)
    {
        return;
    }

    // アニメーション再生中は待機
    if(anim_control->GetNowPlaying((int)CloseEnemyAnimationController::STATE::BIG_HIT))
    {
        return;
    }

    if(!anim_control->GetFlag(CloseEnemyAnimationController::STATE_BIT::BIG_HIT))
    {
        // 立ち上がりアクションへ遷移
        if(auto e_action = GetOwner()->GetComponent<CloseEnemyAction>())
        {
            e_action->SetAction(std::make_shared<CloseEnemyStandUp>());
        }
    }
}
//! @brief 終了
void CloseEnemyKnockback::Exit()
{
    Super::Exit();

    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動再開
        move->SetMove(true);
    }
    if(auto knockback = GetOwner()->GetComponent<Knockback>())
    {
        // ノックバックコンポーネント削除
        GetOwner()->RemoveComponent(knockback);
    }
}

//! @brief GUI表示処理
void CloseEnemyKnockback::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("EnemyKnockback"))
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

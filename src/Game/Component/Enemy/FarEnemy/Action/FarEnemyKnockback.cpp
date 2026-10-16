//===========================================================================
//! @file FarEnemyKnockback.cpp
//! @brief 敵のノックバックコンポーネント
//===========================================================================
#include "FarEnemyKnockback.h"
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/Enemy/Knockback.h>
#include <Game/Component/Enemy/FarEnemy/FarEnemyAnimationController.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyAction.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyIdle.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyDeath.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyFindPlayer.h>

//! @brief 初期化
void FarEnemyKnockback::Init()
{
    Super::Init();
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動停止
        move->SetMove();
    }
    if(auto anim_control = GetOwner()->GetComponent<FarEnemyAnimationController>())
    {
        // アニメーションを設定
        anim_control->SetFlag(FarEnemyAnimationController::STATE_BIT::HIT);
    }
    // ノックバックを追加
    GetOwner()->AddComponent<Knockback>();

    // HPが0以下なら死亡アクションに移行
    if(GetOwner()->GetComponent<ComponentHP>()->GetHP() <= 0)
    {
        if(auto e_action = GetOwner()->GetComponent<FarEnemyAction>())
        {
            e_action->SetAction(std::make_shared<FarEnemyDeath>());
        }
    }
}

//! @brief 更新
void FarEnemyKnockback::Update()
{
    Super::Update();
    if(auto anim_control = GetOwner()->GetComponent<FarEnemyAnimationController>())
    {
        // アニメーション再生中は待機
        if(anim_control->GetNowPlaying((int)FarEnemyAnimationController::STATE::HIT))
        {
            return;
        }
    }
    // プレイヤー探索アクションに移行
    if(auto e_action = GetOwner()->GetComponent<FarEnemyAction>())
    {
        e_action->SetAction(std::make_shared<FarEnemyFindPlayer>());
    }
}
//! @brief 終了
void FarEnemyKnockback::Exit()
{
    Super::Exit();

    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動再開
        move->SetMove(true);
    }
}

//! @brief GUI表示処理
void FarEnemyKnockback::GUI()
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

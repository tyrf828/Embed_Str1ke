//===========================================================================
//! @file CloseEnemyStandUp.cpp
//! @brief 敵の立ち上がりコンポーネント
//===========================================================================
#include "CloseEnemyStandUp.h"
#include <Game/Component/Enemy/CloseEnemy/CloseEnemyAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/Enemy/Knockback.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyAction.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyIdle.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyFindPlayer.h>

//! @brief 初期化
void CloseEnemyStandUp::Init()
{
    Super::Init();
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動停止
        move->SetMove();
    }
    if(auto anim_control = GetOwner()->GetComponent<CloseEnemyAnimationController>())
    {
        // アニメーションを設定
        anim_control->SetFlag(CloseEnemyAnimationController::STATE_BIT::STAND_UP);
    }
}

//! @brief 更新
void CloseEnemyStandUp::Update()
{
    Super::Update();
    if(auto anim_control = GetOwner()->GetComponent<CloseEnemyAnimationController>())
    {
        // アニメーション再生中は待機
        if(anim_control->GetNowPlaying((int)CloseEnemyAnimationController::STATE::STAND_UP))
        {
            return;
        }
    }
    // プレイヤー探索アクションへ遷移
    if(auto e_action = GetOwner()->GetComponent<CloseEnemyAction>())
    {
        e_action->SetAction(std::make_shared<CloseEnemyFindPlayer>());
    }
}
//! @brief 終了
void CloseEnemyStandUp::Exit()
{
    Super::Exit();

    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動再開
        move->SetMove(true);
    }
}

//! @brief GUI表示処理
void CloseEnemyStandUp::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("EnemyStandUp"))
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

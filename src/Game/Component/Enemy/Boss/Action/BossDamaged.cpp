//===========================================================================
//! @file BossDamaged.cpp
//! @brief ボスがダメージ受けたコンポーネント
//===========================================================================
#include "BossDamaged.h"
#include <Game/Component/Enemy/Boss/BossAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Component/Enemy/Boss/Action/BossAction.h>
#include <Game/Component/Enemy/Boss/Action/BossIdle.h>
#include <Game/Component/Enemy/Boss/Action/BossDeath.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
void BossDamaged::Init()
{
    Super::Init();
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 一時的に移動停止
        move->SetMove();
    }
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーション再生
        if(!anim_control->GetFlag(BossAnimationController::STATE_BIT::HIT))
        {
            anim_control->SetFlag(BossAnimationController::STATE_BIT::HIT);
        }
        // すでにダメージアニメーション再生中なら再度再生
        else if(anim_control->GetNowPlaying((int)BossAnimationController::STATE::HIT))
        {
            anim_control->PlayHitAgain();
        }
    }
    if(auto hp = GetOwner()->GetComponent<ComponentHP>())
    {
        // HPが0以下なら死亡アクションへ遷移
        if(hp->GetHP() <= 0)
        {
            if(auto e_action = GetOwner()->GetComponent<BossAction>())
            {
                e_action->SetAction(std::make_shared<BossDeath>());
                return;
            }
        }
    }
    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        // 音を再生
        se->Play("hit");
    }
}

//! @brief 更新
void BossDamaged::Update()
{
    Super::Update();
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーション終了後、前のアクションに戻る
        if(!anim_control->GetFlag(BossAnimationController::STATE_BIT::HIT))
        {
            if(auto e_action = GetOwner()->GetComponent<BossAction>())
            {
                e_action->SetAction(e_action->GetPrevAction());
            }
        }
    }
}

//! @brief 終了
void BossDamaged::Exit()
{
    Super::Exit();

    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動再開
        move->SetMove(true);
    }
}

//! @brief GUI表示処理
void BossDamaged::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("BossDamaged"))
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

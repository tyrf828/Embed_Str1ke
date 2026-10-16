//===========================================================================
//! @file BossFallBack.cpp
//! @brief ボスの後退コンポーネント
//===========================================================================
#include "BossFallBack.h"
#include <Game/Component/Enemy/Boss/BossAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/Enemy/Boss/Action/BossAction.h>
#include <Game/Component/Enemy/Boss/Action/BossJump.h>

//! @brief 初期化
void BossFallBack::Init()
{
    Super::Init();

    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションを設定
        anim_control->SetFlag(BossAnimationController::STATE_BIT::FALL_BACK);
    }
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 後退速度を設定
        move->SetVelocity({0.0f, 0.0f, 0.0f});
        prev_speed_ = move->GetSpeed();
        move->SetSpeed(speed_);
    }
}

//! @brief 更新
void BossFallBack::Update()
{
    Super::Update();
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 後退移動
        float3 backward = GetOwner()->GetVectorAxisZ();
        backward.y      = 0.0f;
        backward        = normalize(backward);
        move->SetVelocity(backward);
    }
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーション終了したらジャンプアクションに遷移
        if(anim_control->IsAnimationTimeInRange(1.3f) && anim_control->GetNowPlaying((int)BossAnimationController::STATE::FALL_BACK))
        {
            if(auto e_action = GetOwner()->GetComponent<BossAction>())
            {
                e_action->SetAction(std::make_shared<BossJump>());
            }
            return;
        }
    }
}

//! @brief 終了
void BossFallBack::Exit()
{
    Super::Exit();
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションフラグをリセット
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::FALL_BACK);
    }
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 速度を元に戻す
        move->SetVelocity({0.0f, 0.0f, 0.0f});
        move->SetSpeed(prev_speed_);
    }
}

//! @brief GUI表示処理
void BossFallBack::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("BossFallBack"))
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

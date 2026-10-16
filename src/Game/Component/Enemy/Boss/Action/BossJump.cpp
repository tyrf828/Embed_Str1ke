//===========================================================================
//! @file BossJump.cpp
//! @brief ボスのジャンプ動作コンポーネント
//===========================================================================
#include "BossJump.h"
#include <Game/Component/Enemy/Boss/BossAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/Enemy/Boss/Action/BossAction.h>
#include <Game/Component/Enemy/Boss/Action/BossIdle.h>
#include <Game/Component/Enemy/Boss/Action/BossDashAttack.h>

//! @brief 初期化
void BossJump::Init()
{
    Super::Init();

    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションを設定
        anim_control->SetFlag(BossAnimationController::STATE_BIT::JUMP);
    }
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        move->SetVelocity({0.0f, 0.0f, 0.0f});
    }
    if(auto col = GetOwner()->GetComponent<ComponentCollisionCapsule>())
    {
        // 重力を無効化
        col->UseGravity(false);
    }
}

//! @brief 更新
void BossJump::Update()
{
    Super::Update();

    // 鉛直投げ上げ公式
    // y = V0 * dt - 0.5 * g * dt * dt
    float gravity  = 9.8f;   // 重力加速度
    elapsed_time_ += GetDeltaTime();
    float force    = jump_force_ * elapsed_time_ - (0.5f * gravity * elapsed_time_ * elapsed_time_);
    if(force < 0.0f)
    {
        // 落下開始したら突進攻撃に遷移
        if(auto e_action = GetOwner()->GetComponent<BossAction>())
        {
            e_action->SetAction(std::make_shared<BossDashAttack>());
        }
        return;
    }
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        move->SetVelocity(float3{0.0f, force, 0.0f});
    }
    // プレイヤーの方向を向く
    GetOwner()->SetRotationToPositionWithLimit(GetPlayer()->GetTranslate(), 5.0f);
}

//! @brief 終了
void BossJump::Exit()
{
    Super::Exit();
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::JUMP);
    }
    if(auto col = GetOwner()->GetComponent<ComponentCollisionCapsule>())
    {
        // 重力を有効化
        col->UseGravity(true);
    }
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        move->SetVelocity(float3{0.0f, 0.0f, 0.0f});
    }
}

//! @brief GUI表示処理
void BossJump::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("BossJump"))
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

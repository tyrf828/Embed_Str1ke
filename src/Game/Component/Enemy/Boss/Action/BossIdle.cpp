//===========================================================================
//! @file BossIdle.cpp
//! @brief ボスの待機コンポーネント
//===========================================================================
#include "BossIdle.h"
#include <Game/Component/Enemy/Boss/BossAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>

//! @brief 初期化
void BossIdle::Init()
{
    Super::Init();

    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        anim_control->ResetAllFlags();
        // アニメーションを設定
        anim_control->SetFlag(BossAnimationController::STATE_BIT::IDLE);
    }
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動を停止
        move->SetVelocity({0.0f, 0.0f, 0.0f});
    }
}

//! @brief 更新
void BossIdle::Update()
{
    Super::Update();
}

//! @brief 終了
void BossIdle::Exit()
{
    Super::Exit();
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションフラグをリセット
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::IDLE);
    }
}

//! @brief GUI表示処理
void BossIdle::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("BossIdle"))
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

//===========================================================================
//! @file FarEnemyIdle.cpp
//! @brief 敵の待機コンポーネント
//===========================================================================
#include "FarEnemyIdle.h"
#include <Game/Component/Enemy/FarEnemy/FarEnemyAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>

//! @brief 初期化
void FarEnemyIdle::Init()
{
    Super::Init();

    if(auto anim_control = GetOwner()->GetComponent<FarEnemyAnimationController>())
    {
        // アニメーションを設定
        anim_control->SetFlag(FarEnemyAnimationController::STATE_BIT::IDLE);
    }
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動を停止
        move->SetVelocity(float3{0.0f, 0.0f, 0.0f});
    }
}

//! @brief 更新
void FarEnemyIdle::Update()
{
    Super::Update();
}

//! @brief 終了
void FarEnemyIdle::Exit()
{
    Super::Exit();
}

//! @brief GUI表示処理
void FarEnemyIdle::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("EnemyIdle"))
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

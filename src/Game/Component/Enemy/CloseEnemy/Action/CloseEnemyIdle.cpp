//===========================================================================
//! @file CloseEnemyIdle.cpp
//! @brief 敵の待機コンポーネント
//===========================================================================
#include "CloseEnemyIdle.h"
#include <Game/Component/Enemy/CloseEnemy/CloseEnemyAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>

//! @brief 初期化
void CloseEnemyIdle::Init()
{
    Super::Init();

    if(auto anim_control = GetOwner()->GetComponent<CloseEnemyAnimationController>())
    {
        // アニメーションを設定
        anim_control->SetFlag(CloseEnemyAnimationController::STATE_BIT::IDLE);
    }
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動を停止
        move->SetVelocity({0.0f, 0.0f, 0.0f});
    }
}

//! @brief 更新
void CloseEnemyIdle::Update()
{
    Super::Update();
}

//! @brief 終了
void CloseEnemyIdle::Exit()
{
    Super::Exit();
}

//! @brief GUI表示処理
void CloseEnemyIdle::GUI()
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

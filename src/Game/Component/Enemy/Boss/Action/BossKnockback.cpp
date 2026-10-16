//===========================================================================
//! @file BossKnockback.cpp
//! @brief ボスのノックバックコンポーネント
//===========================================================================
#include "BossKnockback.h"
#include <Game/Component/Enemy/Boss/BossAnimationController.h>
#include <Game/Component/Enemy/Boss/Action/BossAction.h>
#include <Game/Component/Enemy/Boss/Action/BossIdle.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Component/Enemy/Boss/Action/BossDeath.h>

//! @brief 初期化
void BossKnockback::Init()
{
    Super::Init();

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

    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::HIT);
        anim_control->SetFlag(BossAnimationController::STATE_BIT::KNOCKBACK);
    }
    // プレイヤーの方向を向く
    GetOwner()->SetRotationToPosition(GetPlayer()->GetTranslate());

    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 後方へノックバック
        move->SetVelocity(GetOwner()->GetVectorAxisZ());
    }
}

//! @brief 更新
void BossKnockback::Update()
{
    Super::Update();
    auto anim_control = GetOwner()->GetComponent<BossAnimationController>();
    if(!anim_control)
    {
        return;
    }
    if(anim_control->GetNowPlaying((int)BossAnimationController::STATE::KNOCKBACK))
    {
        is_in_knockback_ = true;
    }

    // アニメーション終了時にアイドル状態へ遷移
    if(is_in_knockback_ && anim_control->IsAnimationTimeInRange(3.50f))
    {
        if(auto e_action = GetOwner()->GetComponent<BossAction>())
        {
            e_action->SetAction(std::make_shared<BossIdle>());
        }
    }
}

//! @brief 終了
void BossKnockback::Exit()
{
    Super::Exit();
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::KNOCKBACK);
    }
}

//! @brief GUI表示処理
void BossKnockback::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("BossKnockback"))
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

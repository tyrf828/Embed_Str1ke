//===========================================================================
//! @file CutScenePlayerAnimationController.cpp
//! @brief 演出プレイヤーのアニメーションコントローラー
//===========================================================================
#include "CutScenePlayerAnimationController.h"
#include <System/Component/ComponentModel.h>
#include <Game/Object/Player/Player.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/Player/Move.h>
#include <Game/Object/Player/Camera.h>

//! @brief 初期化
void CutScenePlayerAnimationController::Init()
{
    Super::Init();

    if(auto model = GetOwner()->GetComponent<ComponentModel>())
    {
        // アニメーションの設定
        model->SetAnimation({
            {"idle",        "data/Game/Model/Player/CutScene/Idle.mv1",                           0, 1.0f},
            {"walk",        "data/Game/Model/Player/CutScene/Walking.mv1",                        0, 0.8f},
            {"attack",      "data/Game/Model/Player/CutScene/Standing Melee Attack Downward.mv1", 0, 1.0f},
            {"pick up",     "data/Game/Model/Player/CutScene/Picking Up.mv1",                     0, 0.8f},
            {"look around", "data/Game/Model/Player/CutScene/Look Around.mv1",                    0, 0.5f},
            {"run back",    "data/Game/Model/Player/CutScene/Running Turn 180.mv1",               0, 1.0f},
            {"run",         "data/Game/Model/Player/Two Cycle Sprint.mv1",                        0, 1.0f},
        });
    }
    anim_flag_max_ = (int)STATE::ANIM_MAX;
}

//! @brief 更新
void CutScenePlayerAnimationController::Update()
{
    Super::Update();

    // モデルコンポーネントを取得
    auto model = GetOwner()->GetComponent<ComponentModel>();
    if(!model)
    {
        return;
    }

    // アニメーションの再生処理
    switch(anim_state_)
    {
    case(int)STATE::IDLE:
        model->PlayAnimationNoSame("idle", true, 1.0f);
        break;
    case(int)STATE::WALK:
        model->PlayAnimationNoSame("walk", true);
        break;
    case(int)STATE::ATTACK:
        model->PlayAnimationNoSame("attack");
        break;
    case(int)STATE::PICK_UP:
        model->PlayAnimationNoSame("pick up", false, 1.0f);
        CheckPlayingResetFlag(STATE_BIT::PICK_UP);
        break;
    case(int)STATE::LOOK_AROUND:
        model->PlayAnimationNoSame("look around", false, 1.0f);
        CheckPlayingResetFlag(STATE_BIT::LOOK_AROUND);
        break;
    case(int)STATE::RUN_BACK:
        model->PlayAnimationNoSame("run back", false, 1.0f);
        break;
    case(int)STATE::RUN:
        model->PlayAnimationNoSame("run", true);
        break;
    }
}

//! @brief GUI表示処理
void CutScenePlayerAnimationController::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("CutScenePlayerAnimationController"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::TreePop();

            ImGui::Text(u8"アニメーションフラグ");
            ImGui::Text(u8"現在のアニメーション状態: %d", anim_state_);
            ImGui::Separator();
            for(int i = 0; i < anim_flag_max_; i++)
            {
                bool flag = GetFlag(1 << i);
                if(ImGui::Checkbox(UNIQUE_TEXT(std::to_string(i).c_str()), &flag))
                {
                    if(flag)
                    {
                        SetFlag(1 << i);
                    }
                    else
                    {
                        ResetFlag(1 << i);
                    }
                }
            }
        }
    }
    ImGui::End();
}

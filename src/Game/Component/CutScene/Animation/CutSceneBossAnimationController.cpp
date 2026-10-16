//===========================================================================
//! @file CutSceneBossAnimationController.cpp
//! @brief 演出ボスのアニメーション制御コントローラー
//===========================================================================
#include "CutSceneBossAnimationController.h"
#include <System/Component/ComponentModel.h>
#include <Game/Object/Player/Player.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/Player/Move.h>
#include <Game/Object/Player/Camera.h>

//! @brief 初期化
void CutSceneBossAnimationController::Init()
{
    Super::Init();

    if(auto model = GetOwner()->GetComponent<ComponentModel>())
    {
        // アニメーションの設定
        model->SetAnimation({
            {"sit",       "data/Game/Model/Boss/CutScene/Sitting Idle.mv1",            0, 1.0f},
            {"stand up",  "data/Game/Model/Boss/CutScene/Stand Up.mv1",                0, 1.0f},
            {"jump",      "data/Game/Model/Boss/CutScene/Jumping Up.mv1",              0, 0.5f},
            {"attack",    "data/Game/Model/Boss/CutScene/Flying Knee Punch Combo.mv1", 0, 0.3f},
            {"knockback", "data/Game/Model/Boss/CutScene/Jump Backward.mv1",           0, 1.0f},
        });
    }
    anim_flag_max_ = (int)STATE::ANIM_MAX;
}

//! @brief 更新
void CutSceneBossAnimationController::Update()
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
    case(int)STATE::SIT:
        model->PlayAnimationNoSame("sit", true);
        break;
    case(int)STATE::STAND_UP:
        model->PlayAnimationNoSame("stand up", false, 1.0f);
        break;
    case(int)STATE::JUMP:
        model->PlayAnimationNoSame("jump");
        break;
    case(int)STATE::ATTACK:
        model->PlayAnimationNoSame("attack");
        break;
    case(int)STATE::KNOCKBACK:
        model->PlayAnimationNoSame("knockback");
        break;
    }
}

//! @brief GUI表示処理
void CutSceneBossAnimationController::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("AnimationController"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::TreePop();
        }
        ImGui::Separator();
        ImGui::Text("Anim State / Flags");

        static const char* STATE_NAMES[] = {"SIT", "STAND_UP", "JUMP", "ATTACK", "KNOCKBACK"};

        // 個別フラグ操作 & 状態選択
        for(int i = 0; i < (int)STATE::ANIM_MAX; i++)
        {
            ImGui::PushID(i);
            bool flag = GetFlag(1 << i);
            if(ImGui::Checkbox("", &flag))
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
            ImGui::SameLine();
            bool selected = (anim_state_ == i);
            if(ImGui::Selectable(UNIQUE_TEXT(STATE_NAMES[i]), selected))
            {
                anim_state_ = i;
                SetFlag(1 << i);   // 選択時はフラグを立てる
            }
            ImGui::PopID();
        }
    }
    ImGui::End();
}

void CutSceneBossAnimationController::PlayOnce(int anim_flag)
{
    // モデルコンポーネントを取得
    auto model = GetOwner()->GetComponent<ComponentModel>();
    if(!model)
    {
        return;
    }

    // アニメーションの再生処理
    switch(anim_flag)
    {
    case(int)STATE::SIT:
        model->PlayAnimation("sit", true);
        break;
    case(int)STATE::STAND_UP:
        model->PlayAnimation("stand up", false, 1.0f);
        break;
    case(int)STATE::JUMP:
        model->PlayAnimation("jump");
        break;
    case(int)STATE::ATTACK:
        model->PlayAnimation("attack");
        break;
    case(int)STATE::KNOCKBACK:
        model->PlayAnimation("knockback");
        break;
    }
}

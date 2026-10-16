//===========================================================================
//! @file FarEnemyAnimationController.h
//! @brief 敵のアニメーション制御コントローラー
//===========================================================================
#include "FarEnemyAnimationController.h"
#include <System/Component/ComponentModel.h>

//! @brief 初期化
void FarEnemyAnimationController::Init()
{
    Super::Init();
    if(auto model = GetOwner()->GetComponent<ComponentModel>())
    {
        model->SetAnimation({
            {"idle",       "data/Game/Model/FarEnemy/Pistol Idle.mv1",          0, 1.0f},
            {"walk",       "data/Game/Model/FarEnemy/Running.mv1",              0, 1.0f},
            {"walk right", "data/Game/Model/FarEnemy/Pistol Strafe Right.mv1",  0, 0.8f},
            {"walk left",  "data/Game/Model/FarEnemy/Pistol Strafe Left.mv1",   0, 0.8f},
            {"walk back",  "data/Game/Model/FarEnemy/Pistol Walk Backward.mv1", 0, 1.0f},
            {"attack",     "data/Game/Model/FarEnemy/Shooting.mv1",             0, 0.5f},
            {"hit",        "data/Game/Model/FarEnemy/Hit Reaction.mv1",         0, 1.5f},
            {"death",      "data/Game/Model/FarEnemy/Flying Back Death.mv1",    0, 1.0f},
        });
    }
    anim_flag_max_ = (int)STATE::ANIM_MAX;
}

//! @brief 更新
void FarEnemyAnimationController::Update()
{
    Super::Update();

    // モデルコンポーネントを取得
    auto model = GetOwner()->GetComponent<ComponentModel>();
    if(!model)
    {
        return;
    }

    float rand_start_time = rand() % 10 / 100.0f;
    switch(anim_state_)
    {
    case(int)STATE::IDLE:
        model->PlayAnimationNoSame("idle", true, 0.5f);
        break;
    case(int)STATE::WALK:
        model->PlayAnimationNoSame("walk", true, 1.0f, rand_start_time);
        break;
    case(int)STATE::WALK_RIGHT:
        model->PlayAnimationNoSame("walk right", true, 1.0f);
        break;
    case(int)STATE::WALK_LEFT:
        model->PlayAnimationNoSame("walk left", true, 1.0f);
        break;
    case(int)STATE::WALK_BACK:
        model->PlayAnimationNoSame("walk back", true, 1.0f);
        break;
    case(int)STATE::ATTACK:
        model->PlayAnimationNoSame("attack", false, 0.5f);
        break;
    case(int)STATE::HIT:
        model->PlayAnimationNoSame("hit", false, 1.5f);
        CheckPlayingResetFlag(STATE_BIT::HIT);
        break;
    case(int)STATE::DEATH:
        model->PlayAnimationNoSame("death", false);
        break;
    default:
        break;
    }
}

//! @brief GUI表示処理
void FarEnemyAnimationController::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("FarEnemyAnimationController"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::TreePop();
            ImGui::Separator();

            ImGui::TextUnformatted("Anim State / Flags");

            static const char* STATE_NAMES[] = {"IDLE", "WALK", "WALK_RIGHT", "WALK_LEFT", "WALK_BACK", "ATTACK", "HIT", "DEATH"};

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
    }
    ImGui::End();
}

//! @brief 現在アニメーションをもう一回再生する
void FarEnemyAnimationController::PlayAgain()
{
    auto model = GetOwner()->GetComponent<ComponentModel>();
    if(!model)
    {
        return;
    }
    switch(anim_state_)
    {
    case(int)STATE::IDLE:
        model->PlayAnimation("idle", true, 0.5f);
        break;
    case(int)STATE::WALK:
        model->PlayAnimation("walk", true);
        break;
    case(int)STATE::WALK_RIGHT:
        model->PlayAnimation("walk right", true);
        break;
    case(int)STATE::WALK_LEFT:
        model->PlayAnimation("walk left", true);
        break;
    case(int)STATE::WALK_BACK:
        model->PlayAnimation("walk back", true);
        break;
    case(int)STATE::ATTACK:
        model->PlayAnimation("attack", false);
        break;
    case(int)STATE::HIT:
        model->PlayAnimation("hit", false);
        break;
    case(int)STATE::DEATH:
        model->PlayAnimation("death", false);
        break;
    default:
        break;
    }
}

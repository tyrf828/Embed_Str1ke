#include "BossAnimationController.h"
#include <System/Component/ComponentModel.h>
#include <filesystem>

//! @brief 初期化
void BossAnimationController::Init()
{
    Super::Init();
    if(auto model = GetOwner()->GetComponent<ComponentModel>())
    {
        model->SetAnimation({
            {"idle",        "data/Game/Model/Boss/Standing Idle.mv1",                    0, 1.0f },
            {"ready",       "data/Game/Model/Boss/Magic Spell Casting.mv1",              0, 1.0f },
            {"walk front",  "data/Game/Model/Boss/Walking.mv1",                          0, 1.0f },
            {"walk right",  "data/Game/Model/Boss/Standing Walk Right.mv1",              0, 1.0f },
            {"walk left",   "data/Game/Model/Boss/Standing Walk Left.mv1",               0, 1.0f },
            {"walk back",   "data/Game/Model/Boss/Standing Walk Back.mv1",               0, 1.0f },
            {"attack",      "data/Game/Model/Boss/Surprise Uppercut.mv1",                0, 1.15f},
            {"kick",        "data/Game/Model/Boss/Side Kick.mv1",                        0, 1.15f},
            {"fall back",   "data/Game/Model/Boss/Standing Dodge Backward.mv1",          0, 1.5f },
            {"jump",        "data/Game/Model/Boss/Jumping Up.mv1",                       0, 0.3f },
            {"dash attack", "data/Game/Model/Boss/Sword And Shield Slash.mv1",           0, 1.15f},
            {"blast",       "data/Game/Model/Boss/Standing 2H Magic Area Attack 02.mv1", 0, 1.15f},
            {"hit",         "data/Game/Model/Boss/Medium Hit To Head.mv1",               0, 1.0f },
            {"knockback",   "data/Game/Model/Boss/Receiving An Uppercut.mv1",            0, 1.0f },
            {"death",       "data/Game/Model/Boss/Falling Back Death.mv1",               0, 1.0f },
            {"slam right",  "data/Game/Model/Boss/Zombie Attack Right.mv1",              0, 1.3f },
            {"slam left",   "data/Game/Model/Boss/Zombie Attack Left.mv1",               0, 1.5f },
            {"combo kick",  "data/Game/Model/Boss/Roundhouse Kick.mv1",                  0, 1.3f },
            {"clap",        "data/Game/Model/Boss/clap.mv1",                             0, 2.0f }
        });
    }
    anim_flag_max_ = (int)STATE::ANIM_MAX;
}

//! @brief 更新
void BossAnimationController::Update()
{
    Super::Update();
    // モデルコンポーネントを取得
    auto model = GetOwner()->GetComponent<ComponentModel>();
    if(!model)
    {
        return;
    }

    switch(anim_state_)
    {
    case(int)STATE::IDLE:
        model->PlayAnimationNoSame("idle", true, 0.5f);
        break;
    case(int)STATE::READY:
        model->PlayAnimationNoSame("ready", false, 0.5f);
        break;
    case(int)STATE::WALK_FRONT:
        model->PlayAnimationNoSame("walk front", true, 0.5f);
        break;
    case(int)STATE::WALK_RIGHT:
        model->PlayAnimationNoSame("walk right", true, 0.5f);
        break;
    case(int)STATE::WALK_LEFT:
        model->PlayAnimationNoSame("walk left", true, 0.5f);
        break;
    case(int)STATE::WALK_BACK:
        model->PlayAnimationNoSame("walk back", true, 0.5f);
        break;
    case(int)STATE::HIT:
        model->PlayAnimationNoSame("hit", false, 0.5f);
        CheckPlayingResetFlag(STATE_BIT::HIT);
        break;
    case(int)STATE::ATTACK:
        model->PlayAnimationNoSame("attack", false, 0.5f);
        break;
    case(int)STATE::KICK:
        model->PlayAnimationNoSame("kick", false, 0.5f);
        break;
    case(int)STATE::FALL_BACK:
        model->PlayAnimationNoSame("fall back", false, 0.5f);
        break;
    case(int)STATE::JUMP:
        model->PlayAnimationNoSame("jump", false, 0.5f);
        break;
    case(int)STATE::DASH_ATTACK:
        model->PlayAnimationNoSame("dash attack", false, 0.5f);
        break;
    case(int)STATE::BLAST:
        model->PlayAnimationNoSame("blast", false, 0.5f);
        break;
    case(int)STATE::SLAM_ATTACK_RIGHT:
        model->PlayAnimationNoSame("slam right", false, 1.0f);
        break;
    case(int)STATE::SLAM_ATTACK_LEFT:
        model->PlayAnimationNoSame("slam left", false, 1.0f);
        break;
    case(int)STATE::COMBO_KICK:
        model->PlayAnimationNoSame("combo kick", false, 1.0f);
        break;
    case(int)STATE::CLAP:
        model->PlayAnimationNoSame("clap", false, 0.5f);
        break;
    case(int)STATE::KNOCKBACK:
        model->PlayAnimationNoSame("knockback", false, 0.5f);
        CheckPlayingResetFlag(STATE_BIT::KNOCKBACK);
        break;
    case(int)STATE::DEATH:
        model->PlayAnimationNoSame("death", false, 0.5f);
        break;
    default:
        break;
    }
}

//! @brief GUI表示処理
void BossAnimationController::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("BossAnimationController"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::TreePop();
            ImGui::Separator();

            ImGui::TextUnformatted("Anim State / Flags");

            static const char* STATE_NAMES[] = {"IDLE",
                                                "READY",
                                                "WALK FRONT",
                                                "WALK RIGHT",
                                                "WALK LEFT",
                                                "WALK BACK",
                                                "HIT",
                                                "ATTACK",
                                                "KICK",
                                                "FALL BACK",
                                                "JUMP",
                                                "DASH ATTACK",
                                                "BLAST",
                                                "SLAM RIGHT",
                                                "SLAM LEFT",
                                                "COMBO KICK",
                                                "CLAP",
                                                "KNOCKBACK",
                                                "DEATH"};

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

//! @brief ヒットアニメーションを再再生
void BossAnimationController::PlayHitAgain()
{
    auto model = GetOwner()->GetComponent<ComponentModel>();
    if(!model)
    {
        return;
    }
    // ヒットアニメーションの場合は再生
    if(anim_state_ == (int)STATE::HIT)
    {
        model->PlayAnimation("hit", false, 0.5f);
    }
}

//===========================================================================
//! @file AnimationController.cpp
//! @brief プレイヤーのアニメーション制御コントローラー
//===========================================================================
#include "AnimationController.h"
#include <System/Component/ComponentModel.h>
#include <Game/Object/Player/Player.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/Player/Move.h>
#include <Game/Object/Player/Camera.h>

//! @brief 初期化
void AnimationController::Init()
{
    Super::Init();

    if(auto model = GetOwner()->GetComponent<ComponentModel>())
    {
        // アニメーションの設定
        model->SetAnimation({
            {"idle",                "data/Game/Model/Player/Standing Idle.mv1",                  0, 1.0f },
            {"walk",                "data/Game/Model/Player/Unarmed Run Forward.mv1",            0, 1.0f },
            {"walk backward",       "data/Game/Model/Player/Unarmed Run Back.mv1",               0, 1.0f },
            {"walk left",           "data/Game/Model/Player/Jog Strafe Left.mv1",                0, 1.0f },
            {"walk right",          "data/Game/Model/Player/Jog Strafe Right.mv1",               0, 1.0f },
            {"walk forward left",   "data/Game/Model/Player/Jog Forward Left.mv1",               0, 1.0f },
            {"walk forward right",  "data/Game/Model/Player/Jog Forward Right.mv1",              0, 1.0f },
            {"walk backward left",  "data/Game/Model/Player/Jog Backward Left.mv1",              0, 1.0f },
            {"walk backward right", "data/Game/Model/Player/Jog Backward Right.mv1",             0, 1.0f },
            {"attack1",             "data/Game/Model/Player/Standing Melee Attack Downward.mv1", 0, 1.9f },
            {"attack2",             "data/Game/Model/Player/Sword And Shield Slash.mv1",         0, 1.3f },
            {"attack3",             "data/Game/Model/Player/Standing Melee Attack 360 Low.mv1",  0, 2.05f},
            {"death",               "data/Game/Model/Player/Standing Death Backward 01.mv1",     0, 1.0f },
            {"run",                 "data/Game/Model/Player/Two Cycle Sprint.mv1",               0, 1.0f },
            {"small hit",           "data/Game/Model/Player/Head Hit.mv1",                       0, 2.0f },
            {"big hit",             "data/Game/Model/Player/Rifle Hit To Back.mv1",              0, 1.2f },
            {"stand up",            "data/Game/Model/Player/Stand Up.mv1",                       0, 1.5f },
            {"drink",               "data/Game/Model/Player/Drinking.mv1",                       0, 3.0f },
            {"ultimate dash",       "data/Game/Model/Player/Female Locomotion Pose.mv1",         0, 1.0f },
            {"ultimate finish",     "data/Game/Model/Player/Sword And Shield Idle.mv1",          0, 1.5f },
        });
    }
    anim_flag_max_ = (int)STATE::ANIM_MAX;
}

//! @brief 更新
void AnimationController::Update()
{
    Super::Update();

    // モデルコンポーネントを取得
    auto model = GetOwner()->GetComponent<ComponentModel>();
    if(!model)
    {
        return;
    }

    // 移動コンポーネントを取得
    auto move = GetOwner()->GetComponent<Move>();

    // 現在のアニメーションどれにするか決める処理
    for(int i = 0; i < (int)STATE::ANIM_MAX; i++)
    {
        if(anim_flag_ & (1 << i))
        {
            anim_state_ = i;
        }
    }

    // ロックしている時ターゲットに向かないようにする
    SetFaceTarget();

    // アニメーションの再生処理
    switch(anim_state_)
    {
    case(int)STATE::IDLE:
        // ロックしている時ターゲットに向く
        SetFaceTarget(true);
        // アニメーションを再生
        model->PlayAnimationNoSame("idle", true, 0.5f);
        break;
    case(int)STATE::WALK:
        if(move)
        {
            // ロックしている時ターゲットに向く
            SetFaceTarget(true);

            // フリーカメラの時
            if(!Scene::Object::Get<Camera>("Camera")->IsCameraLock())
            {
                // モデルを入力方向に向かせる
                model->SetRotationToVectorWithLimit(move->GetDir(), 15.0f);
                // アニメーションを再生
                model->PlayAnimationNoSame("walk", true, 0.5f);
            }
            else
            {
                // キー入力を取得
                float3 dir = {0, 0, 0};
                if(move->GetKey(Move::WALK_KEY_STATUS::KEY_FRONT))
                {
                    dir += {0, 0, -1};
                }
                if(move->GetKey(Move::WALK_KEY_STATUS::KEY_BACK))
                {
                    dir += {0, 0, 1};
                }
                if(move->GetKey(Move::WALK_KEY_STATUS::KEY_RIGHT))
                {
                    dir += {-1, 0, 0};
                }
                if(move->GetKey(Move::WALK_KEY_STATUS::KEY_LEFT))
                {
                    dir += {1, 0, 0};
                }

                // キー入力に応じてアニメーションを再生
                if(dir.z < 0 && dir.x == 0)
                {
                    model->PlayAnimationNoSame("walk", true);
                }
                if(dir.z > 0 && dir.x == 0)
                {
                    model->PlayAnimationNoSame("walk backward", true);
                }
                if(dir.x < 0 && dir.z == 0)
                {
                    model->PlayAnimationNoSame("walk right", true);
                }
                if(dir.x > 0 && dir.z == 0)
                {
                    model->PlayAnimationNoSame("walk left", true);
                }
                if(dir.z < 0 && dir.x < 0)
                {
                    model->PlayAnimationNoSame("walk forward right", true);
                }
                if(dir.z < 0 && dir.x > 0)
                {
                    model->PlayAnimationNoSame("walk forward left", true);
                }
                if(dir.z > 0 && dir.x < 0)
                {
                    model->PlayAnimationNoSame("walk backward right", true);
                }
                if(dir.z > 0 && dir.x > 0)
                {
                    model->PlayAnimationNoSame("walk backward left", true);
                }
            }
        }
        break;
    case(int)STATE::RUN:
        // モデルを入力方向に向かせる
        model->SetRotationToVectorWithLimit(move->GetDir(), 15.0f);
        // アニメーションを再生
        model->PlayAnimationNoSame("run", true, 0.5f);
        break;
    case(int)STATE::ATTACK1:
        // アニメーションを再生
        model->PlayAnimationNoSame("attack1", false);
        // アニメーション終了際にフラグを削除
        CheckPlayingResetFlag(STATE_BIT::ATTACK1);
        break;
    case(int)STATE::ATTACK2:
        // アニメーションを再生
        model->PlayAnimationNoSame("attack2", false);
        // アニメーション終了際にフラグを削除
        CheckPlayingResetFlag(STATE_BIT::ATTACK2);
        break;
    case(int)STATE::ATTACK3:
        // アニメーションを再生
        model->PlayAnimationNoSame("attack3", false);
        // アニメーション終了際にフラグを削除
        CheckPlayingResetFlag(STATE_BIT::ATTACK3);
        break;
    case(int)STATE::REGEN:
        // アニメーションを再生
        model->PlayAnimationNoSame("drink");
        // アニメーション終了際にフラグを削除
        CheckPlayingResetFlag(STATE_BIT::REGEN);
        break;
    case(int)STATE::ULTIMATE_DASH:
        // アニメーションを再生
        model->PlayAnimationNoSame("ultimate dash", true);

        break;
    case(int)STATE::ULTIMATE_FINISH:
        // アニメーションを再生
        model->PlayAnimationNoSame("ultimate finish");
        // アニメーション終了際にフラグを削除
        CheckPlayingResetFlag(STATE_BIT::ULTIMATE_FINISH);
        break;
    case(int)STATE::SMALL_HIT:
        // アニメーションを再生
        model->PlayAnimationNoSame("small hit");
        // アニメーション終了際にフラグを削除
        CheckPlayingResetFlag(STATE_BIT::SMALL_HIT);
        break;
    case(int)STATE::BIG_HIT:
        // アニメーションを再生
        model->PlayAnimationNoSame("big hit");
        if(!model->IsPlaying())
        {
            // アニメーション終了際にフラグを削除
            ResetFlag(STATE_BIT::BIG_HIT);
            // 起き上がりアニメーションのフラグを立つ
            SetFlag(STATE_BIT::STAND_UP);
        }
        break;
    case(int)STATE::STAND_UP:
        // アニメーションを再生
        model->PlayAnimationNoSame("stand up", false, 0.5f);
        // アニメーション終了際にフラグを削除
        CheckPlayingResetFlag(STATE_BIT::STAND_UP);
        break;
    case(int)STATE::DEATH:
        // アニメーションを再生
        model->PlayAnimationNoSame("death");
        break;
    }

    // カメラを取得
    if(auto camera = Scene::Object::Get<Camera>("Camera"))
    {
        // カメラロック時
        if(camera->IsCameraLock())
        {
            // ターゲットに向くか
            if(IsFaceTarget())
            {
                // プレイヤーをカメラの前平面ベクトルに向かせる
                auto target_vector = camera->GetPlaneFront();
                model->SetRotationToVectorOnParentWithLimit(target_vector, 5.0f);
            }
        }
    }
}

//! @brief GUI表示処理
void AnimationController::GUI()
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

            ImGui::Text("Anim State / Flags");

            static const char* STATE_NAMES[] = {"IDLE",
                                                "WALK",
                                                "WALK_BACKWARD",
                                                "WALK_LEFT",
                                                "WALK_RIGHT",
                                                "RUN",
                                                "ATTACK1",
                                                "ATTACK2",
                                                "ATTACK3",
                                                "REGEN",
                                                "ULTIMATE_DASH",
                                                "ULTIMATE_FINISH",
                                                "SMALL_HIT",
                                                "BIG_HIT",
                                                "STAND_UP",
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

//! @brief カメラが敵にロックする時ターゲットの方向を向くを設定
//! @param is_face_target ターゲットの方向を向くか
void AnimationController::SetFaceTarget(bool is_face_target)
{
    is_face_target_ = is_face_target;
}

//! @brief カメラが敵にロックする時ターゲットの方向を向いているか
//! @retval true 向いている
//! @retval false 向いていない
bool AnimationController::IsFaceTarget() const
{
    return is_face_target_;
}

//===========================================================================
//! @file FarEnemyThreat.cpp
//! @brief 敵の威嚇コンポーネント
//===========================================================================
#include "FarEnemyThreat.h"
#include <Game/Component/Enemy/FarEnemy/FarEnemyAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyAction.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyIdle.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyFindPlayer.h>
#include <Game/Component/Navmesh.h>

//! @brief 初期化
void FarEnemyThreat::Init()
{
    Super::Init();
    change_timer_ = GetRandomF(CHANGE_DIR_MAX_) + 1.0f;
    if(GetRand(1) == 0)
    {
        strafe_dir_ *= -1;
    }
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        prev_action_speed_ = move->GetSpeed();
        move->SetSpeed(STRAFE_SPEED_);
    }
    if(auto navmesh = GetOwner()->GetComponent<Navmesh>())
    {
        navmesh->UseNavmesh(false);
    }
}

//! @brief 更新
void FarEnemyThreat::Update()
{
    Super::Update();

    auto player = Scene::Object::Get<Player>();
    if(!player)
    {
        return;
    }

    auto move         = GetOwner()->GetComponent<EnemyMove>();
    auto anim_control = GetOwner()->GetComponent<FarEnemyAnimationController>();

    if(!move || !anim_control)
    {
        return;
    }

    float3 player_pos = player->GetTranslate();

    float3 player_dir = player_pos - GetOwner()->GetTranslate();
    float dist        = GetPlayerDistance();

    player_dir = normalize(player_dir);

    // プレイヤーの方向を向く
    GetOwner()->SetRotationToPositionWithLimit(player_pos, 5.0f);

    float3 vel = {0, 0, 0};

    // 距離が近すぎる場合は後退
    if(dist < THREAT_MIN_DISTANCE_)
    {
        back_safe_distance_ = true;
    }

    if(back_safe_distance_)
    {
        // 後退
        vel = -player_dir;
        anim_control->SetFlag(FarEnemyAnimationController::STATE_BIT::WALK_BACK);
        if(dist > THREAT_MIN_DISTANCE_ + THREAT_ADJUST_DISTANCE_)
        {
            anim_control->ResetFlag(FarEnemyAnimationController::STATE_BIT::WALK_BACK);
            back_safe_distance_ = false;
        }
        move->SetSpeed(STRAFE_BACK_SPEED_);
    }
    else
    {
        // 横移動
        move->SetSpeed(STRAFE_SPEED_);
        float3 up    = {0.0f, 1.0f, 0.0f};
        float3 right = normalize(cross(up, player_dir));
        float3 left  = -right;
        if(strafe_dir_ > 0)
        {
            vel = left;
        }
        else
        {
            vel = right;
        }
        // アニメーション設定
        if(strafe_dir_ > 0)
        {
            anim_control->SetFlag(FarEnemyAnimationController::STATE_BIT::WALK_LEFT);
            anim_control->ResetFlag(FarEnemyAnimationController::STATE_BIT::WALK_RIGHT);
        }
        else
        {
            anim_control->SetFlag(FarEnemyAnimationController::STATE_BIT::WALK_RIGHT);
            anim_control->ResetFlag(FarEnemyAnimationController::STATE_BIT::WALK_LEFT);
        }

        // 一定時間ごとに方向転換
        change_timer_ -= GetDeltaTime();
        if(change_timer_ <= 0)
        {
            change_timer_  = GetRandomF(CHANGE_DIR_MAX_) + 1.0f;
            strafe_dir_   *= -1;
        }
    }
    // 速度設定
    move->SetVelocity(vel);
}

//! @brief 終了
void FarEnemyThreat::Exit()
{
    Super::Exit();

    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 速度を元に戻す
        move->SetSpeed(prev_action_speed_);
        move->SetTarget(nullptr);
    }
    if(auto anim_control = GetOwner()->GetComponent<FarEnemyAnimationController>())
    {
        // アニメーションフラグリセット
        anim_control->ResetFlag(FarEnemyAnimationController::STATE_BIT::WALK_BACK);
        anim_control->ResetFlag(FarEnemyAnimationController::STATE_BIT::WALK_LEFT);
        anim_control->ResetFlag(FarEnemyAnimationController::STATE_BIT::WALK_RIGHT);
    }
    if(auto navmesh = GetOwner()->GetComponent<Navmesh>())
    {
        // ナビメッシュを有効化
        navmesh->UseNavmesh(true);
    }
}

//! @brief GUI表示処理
void FarEnemyThreat::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("EnemyThreat"))
        {
            ImGui::Text(u8"威嚇");
            ImGui::Text(u8"方向: %s", strafe_dir_ > 0 ? u8"左回り" : u8"右回り");
            if(ImGui::Button(UNIQUE_TEXT(u8"方向切替")))
            {
                strafe_dir_   *= -1;
                change_timer_  = 0.0f;
            }

            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

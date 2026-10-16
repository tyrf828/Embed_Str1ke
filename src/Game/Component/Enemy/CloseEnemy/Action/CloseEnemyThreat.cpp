//===========================================================================
//! @file CloseEnemyThreat.cpp
//! @brief 敵の威嚇コンポーネント
//===========================================================================
#include "CloseEnemyThreat.h"
#include <Game/Component/Enemy/CloseEnemy/CloseEnemyAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyAction.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyIdle.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyFindPlayer.h>
#include <Game/Component/Navmesh.h>

//! @brief 初期化
void CloseEnemyThreat::Init()
{
    Super::Init();

    // 方向転換タイマー初期化
    change_timer_ = GetRandomF(CHANGE_DIR_MAX_) + 1.0f;
    // ランダムで移動方向決定
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
        // ナビメッシュを無効化
        navmesh->UseNavmesh(false);
    }
}

//! @brief 更新
void CloseEnemyThreat::Update()
{
    Super::Update();

    auto player = Scene::Object::Get<Player>();
    if(!player)
    {
        return;
    }

    auto move         = GetOwner()->GetComponent<EnemyMove>();
    auto anim_control = GetOwner()->GetComponent<CloseEnemyAnimationController>();

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

    float3 vel = {0.0f, 0.0f, -1.0f};

    if(dist < THREAT_MIN_DISTANCE_)
    {
        // ランダムで後退距離を変える
        float offset        = GetRandomF(GetThreatMaxDistance() - GetThreatMinDistance());
        back_safe_distance_ = true;
        // 最小距離 + ランダム距離まで下がる
        back_target_dist_ = THREAT_MIN_DISTANCE_ + offset;
    }

    if(back_safe_distance_)
    {
        vel = -player_dir;
        anim_control->SetFlag(CloseEnemyAnimationController::STATE_BIT::WALK_BACK);
        if(dist > back_target_dist_)
        {
            anim_control->ResetFlag(CloseEnemyAnimationController::STATE_BIT::WALK_BACK);
            back_safe_distance_ = false;
        }
    }
    else
    {
        // 横移動
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
        // 横移動アニメーション
        if(strafe_dir_ > 0)
        {
            anim_control->SetFlag(CloseEnemyAnimationController::STATE_BIT::WALK_LEFT);
            anim_control->ResetFlag(CloseEnemyAnimationController::STATE_BIT::WALK_RIGHT);
        }
        else
        {
            anim_control->SetFlag(CloseEnemyAnimationController::STATE_BIT::WALK_RIGHT);
            anim_control->ResetFlag(CloseEnemyAnimationController::STATE_BIT::WALK_LEFT);
        }

        // 一定時間ごとに方向転換
        change_timer_ -= GetDeltaTime();
        if(change_timer_ <= 0)
        {
            change_timer_  = GetRandomF(CHANGE_DIR_MAX_) + 1.0f;
            strafe_dir_   *= -1;
        }
    }
    // 移動速度設定
    move->SetVelocity(vel);
}

//! @brief 終了
void CloseEnemyThreat::Exit()
{
    Super::Exit();

    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動速度を元に戻す
        move->SetSpeed(prev_action_speed_);
        move->SetTarget(nullptr);
    }
    if(auto anim_control = GetOwner()->GetComponent<CloseEnemyAnimationController>())
    {
        // アニメーションフラグリセット
        anim_control->ResetFlag(CloseEnemyAnimationController::STATE_BIT::WALK_BACK);
        anim_control->ResetFlag(CloseEnemyAnimationController::STATE_BIT::WALK_LEFT);
        anim_control->ResetFlag(CloseEnemyAnimationController::STATE_BIT::WALK_RIGHT);
    }
    if(auto navmesh = GetOwner()->GetComponent<Navmesh>())
    {
        // ナビメッシュを有効化
        navmesh->UseNavmesh(true);
    }
}

//! @brief GUI表示処理
void CloseEnemyThreat::GUI()
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

//===========================================================================
//! @file ReciprocalVelocityObstacles.cpp
//! @brief 敵同士衝突回避アルゴリズム
//===========================================================================
#include "ReciprocalVelocityObstacles.h"
#include <Game/Object/Enemy/EnemyBase.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/Navmesh.h>
#include <System/Lib.h>

//! @brief 初期化
void RVO::Init()
{
    Super::Init();
    bias_ = GetRandomF(-1.0f, 1.0f);

    SetProc("RVO:Update", [this]() { Update(); }, ProcTiming::Update, static_cast<ProcPriority>(2000));
}

//! @brief 更新
void RVO::Update()
{
    Super::Update();
    enemy_list_.clear();

    auto move = GetOwner()->GetComponent<EnemyMove>();
    auto col  = GetOwner()->GetComponent<ComponentCollisionCapsule>();
    if(!move || !col)
    {
        return;
    }

    preferred_velocity_ = move->GetVelocity();

    for(auto& near_enemy : Scene::Object::GetArray<EnemyBase>())
    {
        if(near_enemy != GetOwnerPtr())
        {
            auto near_move = near_enemy->GetComponent<EnemyMove>();
            // 移動しない敵は無視
            if(!near_move || near_move->GetVelocityLength() < FLT_EPSILON)
            {
                continue;
            }
            // 一定距離内の敵をリストに追加
            float distance = length(near_enemy->GetTranslate() - GetOwner()->GetTranslate());
            if(distance < col->GetRadius() * 15)
            {
                enemy_list_.push_back(near_enemy.get());
            }
        }
    }

    float3 total_avoidance = float3{0, 0, 0};
    for(auto& other : enemy_list_)
    {
        total_avoidance += AdjustVelocity(other);
    }

    if(dot(preferred_velocity_, preferred_velocity_) < (float1)FLT_EPSILON)
    {
        return;
    }
    float3 right          = normalize(cross(float3{0, 1, 0}, preferred_velocity_));
    float3 target_pos     = move->GetTargetPos();
    float dist            = length(target_pos - GetOwner()->GetTranslate());
    const float MAX_DIST  = 100.0f;
    float factor          = std::clamp(dist / MAX_DIST, 0.0f, 1.0f);
    total_avoidance      += right * bias_ * factor;

    if(auto nav = GetOwner()->GetComponent<Navmesh>())
    {
        float3 next_pos       = GetOwner()->GetTranslate() + (preferred_velocity_ + total_avoidance) * move->GetSpeed() * GetDeltaTime();
        float3 clamped        = nav->GetNearestPos(next_pos);
        float3 final_velocity = clamped - GetOwner()->GetTranslate();
        if(dot(final_velocity, final_velocity) > (float1)FLT_EPSILON)
        {
            preferred_velocity_ = normalize(final_velocity);
        }
    }

    // 総合速度
    float3 desired_velocity = preferred_velocity_ + total_avoidance;
    desired_velocity        = ClampLength(desired_velocity, 1.0f);

    // 加速度的に近づける
    float3 to_target    = desired_velocity - move->GetVelocity();
    float3 acceleration = ClampLength(to_target, 0.2f);
    acceleration.y      = 0.0f;
    move->SetVelocity(move->GetVelocity() + acceleration);
}

//! @brief 描画
void RVO::LateDraw()
{
    Super::LateDraw();

    if(!use_debug_)
    {
        return;
    }
    DrawLine3D(
        cast(GetOwner()->GetTranslate()), cast(GetOwner()->GetTranslate() + GetOwner()->GetComponent<EnemyMove>()->GetVelocity() * 10), GetColor(255, 0, 0));

    for(auto& other : enemy_list_)
    {
        DrawSphere3D(cast(other->GetTranslate()), other->GetRadius(), 64, GetColor(0, 255, 0), GetColor(0, 255, 0), true);
        DrawLine3D(cast(GetOwner()->GetTranslate()), cast(other->GetTranslate()), GetColor(0, 0, 255));
    }
}

//! @brief GUI表示処理
void RVO::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("RVO"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::Checkbox("Use Debug", &use_debug_);
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//! @brief 速度を調整
//! @param other 他の敵
//! @return 調整された速度
float3 RVO::AdjustVelocity(EnemyBase* other)
{
    const float time_horizon = 2.0f;

    float3 pos1 = GetOwner()->GetTranslate();
    float3 pos2 = other->GetTranslate();
    float3 vel1 = GetOwner()->GetComponent<EnemyMove>()->GetVelocity();
    float3 vel2 = other->GetComponent<EnemyMove>()->GetVelocity();

    if(other->GetComponent<EnemyMove>()->GetVelocityLength() < FLT_EPSILON)
    {
        return float3{0, 0, 0};
    }

    float3 rel_pos = pos2 - pos1;
    float3 rel_vel = vel1 - vel2;

    float combined_radius = GetOwner()->GetComponent<ComponentCollisionCapsule>()->GetRadius() * 2.0f + other->GetRadius() * 2.0f;

    // 中心点を計算
    float3 vo_center = rel_pos / time_horizon;

    // 距離
    float dist = sqrt(dot(rel_pos, rel_pos));

    // すでに接触している場合、強制回避
    if(dist < combined_radius)
    {
        float3 push_dir = normalize(rel_pos);
        float3 side     = normalize(cross(float3{0, 1, 0}, push_dir));
        float overlap   = combined_radius - dist;
        return -push_dir * overlap + side * bias_;
    }

    // 内積で角度を判定する
    float3 cone_dir   = normalize(rel_pos);
    float theta       = asin(std::clamp(combined_radius / dist, 0.0f, 1.0f));
    float rel_vel_len = sqrt(dot(rel_vel, rel_vel));

    // 相対速度がほぼゼロ、またはコーンと逆方向なら回避不要
    float rel_dot = (float)dot(rel_vel, cone_dir);
    if(rel_vel_len < FLT_EPSILON || rel_dot <= 0.0f)
    {
        return float3{0, 0, 0};
    }

    float cos_angle = (float)dot(rel_vel / rel_vel_len, cone_dir);
    float angle     = acos(std::clamp(cos_angle, -1.0f, 1.0f));

    if(angle >= theta)
    {
        // 回避不要
        return float3{0, 0, 0};
    }

    // 回避方向を計算
    float3 side_dir = normalize(cross(cross(cone_dir, rel_vel), cone_dir));
    if(dot(side_dir, rel_vel) < (float1)FLT_EPSILON)
    {
        side_dir = -side_dir;
    }

    // 回避ベクトルの大きさ
    float penetration_depth = combined_radius / time_horizon - dot(rel_vel, cone_dir);
    float avoid_strength    = std::clamp(penetration_depth, 0.0f, 1.0f);

    return side_dir * avoid_strength;
}

//===========================================================================
//! @file EnemyMove.cpp
//! @brief 敵移動コンポーネント
//===========================================================================
#include "EnemyMove.h"
#include <Game/Component/Navmesh.h>

//! @brief 初期化
void EnemyMove::Init()
{
    Super::Init();
}

//! @brief 更新
void EnemyMove::Update()
{
    Super::Update();

    // ターゲットへのベクトルを計算
    if(target_.lock() && !is_set_pos_)
    {
        velocity_ = target_.lock()->GetTranslate() - GetOwner()->GetTranslate();
    }
    else if(is_set_pos_ && target_.expired())
    {
        velocity_ = target_pos_ - GetOwner()->GetTranslate();
    }

    // 移動量が小さい場合は停止
    if(dot(velocity_, velocity_) < (float1)FLT_EPSILON)
    {
        velocity_ = {0.0f, 0.0f, 0.0f};
    }
    else
    {
        // 正規化
        velocity_ = normalize(velocity_);
    }
}

//! @brief 更新後処理
void EnemyMove::PostUpdate()
{
    Super::PostUpdate();
    // 移動処理
    if(can_move_ && GetVelocityLength() > FLT_EPSILON)
    {
        GetOwner()->AddTranslate(velocity_ * speed_, move_local_);
    }
}

//! @brief GUI表示処理
void EnemyMove::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("EnemyMove"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }

            ImGui::Text("Speed: %f", speed_);
            ImGui::Text("Velocity: (%f, %f, %f)", (float)velocity_.x, (float)velocity_.y, (float)velocity_.z);

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//! @brief ターゲットを設定
//! @param target ターゲットオブジェクト
//! @param move_local ローカル座標で移動するか
void EnemyMove::SetTarget(ObjectPtr target, bool move_local)
{
    if(!target)
    {
        target_.reset();
        velocity_   = {0.0f, 0.0f, 0.0f};
        is_set_pos_ = false;
        return;
    }
    target_     = target;
    velocity_   = normalize(target_.lock()->GetTranslate() - GetOwner()->GetTranslate());
    move_local_ = move_local;
    is_set_pos_ = false;
}

//! @brief ターゲット座標を設定
//! @param pos ターゲット座標
//! @param move_local ローカル座標で移動するか
void EnemyMove::SetPosTarget(float3& pos, bool move_local)
{
    velocity_   = normalize(target_pos_ - GetOwner()->GetTranslate());
    target_pos_ = pos;
    move_local_ = move_local;
    is_set_pos_ = true;
    target_.reset();
}

//! @brief 移動ベクトルを設定
//! @param vel 移動ベクトル
void EnemyMove::SetVelocity(float3 vel)
{
    if(dot(vel, vel) < (float1)FLT_EPSILON)
    {
        velocity_ = {0.0f, 0.0f, 0.0f};
        return;
    }
    velocity_ = normalize(vel);
}

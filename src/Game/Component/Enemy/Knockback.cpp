//===========================================================================
//! @file Knockback.cpp
//! @brief 敵のノックバック
//===========================================================================
#include "Knockback.h"
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentModel.h>

//! @brief 初期化
void Knockback::Init()
{
    Super::Init();

    damping_ratio_ = 0.1f;
    if(auto model = GetOwner()->GetComponent<ComponentModel>())
    {
        // ノックバックアニメーション再生
        model->PlayAnimationNoSame("big hit");
    }
}

//! @brief 更新
void Knockback::Update()
{
    Super::Update();
    if(auto model = GetOwner()->GetComponent<ComponentModel>())
    {
        // 接地してアニメーションが終了したら削除
        if(landed_ && !model->IsPlaying())
        {
            GetOwner()->RemoveComponent(shared_from_this());
            return;
        }
    }
}

//! @brief 遅延描画
void Knockback::LateDraw()
{
    Super::LateDraw();
}

//! @brief GUI表示処理
void Knockback::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("Knockback"))
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

//! @brief 終了
void Knockback::Exit()
{
    Super::Exit();
}

//! @brief 敵側の OnHit から呼ばれる通知
//! @param hit_info 衝突情報
void Knockback::IsHitGround(const ComponentCollision::HitInfo& hit_info)
{
    if(static_cast<u32>(hit_info.hit_collision_->GetCollisionGroup()) & static_cast<u32>(ComponentCollision::CollisionGroup::GROUND))
    {
        landed_ = true;
    }
}

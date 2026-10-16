//===========================================================================
//! @file StageTrigger.cpp
//! @brief ステージのトリガー
//===========================================================================
#include "StageTrigger.h"
#include <System/Component/ComponentCollisionBox.h>
#include <System/Component/ComponentEffect.h>

//! @brief 初期化
//! @return 初期化済み
bool StageTrigger::Init()
{
    Super::Init();
    // 衝突のコンポーネントを追加
    AddComponent<ComponentCollisionBox>()
        ->SetCollisionGroup(ComponentCollision::CollisionGroup::STAGE_TRIGGER)
        ->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER)
        ->SetOverlapCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER)
        ->SetSize(float3{30.0f, 30.0f, 30.0f});
    return true;
}

//! @brief 衝突時の処理
void StageTrigger::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    // プレイヤーと衝突した場合
    if(static_cast<u32>(hit_info.hit_collision_->GetCollisionGroup()) & static_cast<u32>(ComponentCollision::CollisionGroup::PLAYER))
    {
        is_hit_ = true;
    }

    Super::OnHit(hit_info);
}
//! @brief 衝突判定
//! @retval true している
//! @retval false していない
bool StageTrigger::IsHit()
{
    return is_hit_;
}

void StageTrigger::SetSize(float3 size)
{
    auto col = GetComponent<ComponentCollisionBox>();
    if(col)
    {
        col->SetSize(size);
    }
}

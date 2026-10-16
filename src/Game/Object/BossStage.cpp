//===========================================================================
//! @file BossStage.cpp
//! @brief ボスステージ
//===========================================================================
#include "BossStage.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionModel.h>

//! @brief 初期化
//! @return 初期化済み
bool BossStage::Init()
{
    Super::Init();
    SetName("BossStage");

    // モデルを追加
    auto model = AddComponent<ComponentModel>("data/Game/Model/Stage/BossStage.mv1");
    model->UseReceivesDecal(1);

    // コリジョンを追加
    auto col = AddComponent<ComponentCollisionModel>();
    col->AttachToModel();
    col->SetCollisionGroup(ComponentCollision::CollisionGroup::GROUND);
    col->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER | (u32)ComponentCollision::CollisionGroup::ITEM |
                              (u32)ComponentCollision::CollisionGroup::ENEMY | (u32)ComponentCollision::CollisionGroup::ETC);
    SetScaleAxisXYZ({20, 20, 20});

    return true;
}

//! @brief 終了
void BossStage::Exit()
{
    Super::Exit();
}

//===========================================================================
//! @file Stage.cpp
//! @brief ステージ
//===========================================================================
#include "Stage.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionBox.h>

//! @brief 初期化
//! @return 初期化済み
bool Stage::Init()
{
    Super::Init();
    // ステージモデルの読み込み
    auto model = AddComponent<ComponentModel>("data/Game/Model/Stage/LODMap.mv1");
    model->UseReceivesDecal(1);
    // モデルコリジョンのアタッチ
    AddComponent<ComponentCollisionModel>()->AttachToModel();

    // 壁の作成
    auto wall = Scene::Object::Create<Object>("Wall")->SetTranslate(float3{-664.0f, 140.0f, -307.0f});
    wall->AddComponent<ComponentCollisionBox>()
        ->SetSize(float3{80.0f, 80.0f, 100.0f})
        ->SetMass(-1.0f)
        ->SetCollisionGroup(ComponentCollision::CollisionGroup::GROUND)
        ->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER | (u32)ComponentCollision::CollisionGroup::ETC |
                               (u32)ComponentCollision::CollisionGroup::ITEM)
        ->UseGravity(false);
    return true;
}

//! @brief 更新
void Stage::Update()
{
    Super::Update();
}

//! @brief 更新後処理
void Stage::PostUpdate()
{
    Super::PostUpdate();
}

//! @brief 終了
void Stage::Exit()
{
    Super::Exit();
}

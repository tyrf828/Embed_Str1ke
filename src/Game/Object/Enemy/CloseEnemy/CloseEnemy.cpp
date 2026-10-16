//===========================================================================
//! @file CloseEnemy.cpp
//! @brief 近距離敵
//===========================================================================
#include "CloseEnemy.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyAction.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/WeaponAttack.h>
#include <System/Component/ComponentAttachModel.h>
#include <Game/Component/Sound.h>
#include <Game/Component/ReciprocalVelocityObstacles.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/Navmesh.h>
#include <Game/Component/Enemy/Knockback.h>
#include <Game/Component/Enemy/CloseEnemy/CloseEnemyAnimationController.h>
#include <Game/Component/Enemy/AttackParam.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyIdle.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyFindPlayer.h>
#include <Game/Object/Manager/EnemyMentalManager.h>

//! @brief 初期化
//! @return 初期化済み
bool CloseEnemy::Init()
{
    Super::Init();

    SetName("CloseEnemy");

    // モデルコンポーネントを追加
    auto model = AddComponent<ComponentModel>("data/Game/Model/CloseEnemy/Ely By K.Atienza.mv1");   // モデルコンポーネント

    // スケールを設定
    SetScaleAxisXYZ({1.5f, 1.5f, 1.5f});

    // コリジョン情報を設定
    collision_info_.radius_ = 2.3f;
    collision_info_.height_ = 13.5f;
    collision_info_.mass_   = 10.0f;
    // コリジョンコンポーネントを追加
    ResetCollision();

    // 行動コンポーネントを追加
    AddComponent<CloseEnemyAction>();
    // HPコンポーネントを追加
    AddComponent<ComponentHP>()->SetMaxHP(MAX_HP_)->SetHP(MAX_HP_);
    // 音の読み込み
    AddComponent<Sound>()->Load({
        {"walk",  "data/Game/SE/Enemy/アスファルトの上を走る1.mp3", 128.0f, 0.5f},
        {"hit",   "data/Game/SE/Enemy/Close/「ぐっ！」.mp3",        128.0f, 0.7f},
        {"death", "data/Game/SE/Enemy/Close/「くおぉっ！」.mp3",    128.0f, 1.0f},
    });

    AddComponent<CloseEnemyAnimationController>();
    AddComponent<EnemyMove>();
    AddComponent<AttackParam>();
    AddComponent<Navmesh>();
    AddComponent<RVO>();

    //　剣を生成
    weapon_sword_ = Scene::Object::Create<EnemySword>();
    weapon_sword_->SetOwner(shared_from_this());
    weapon_sword_->AttachModel("mixamorig:RightHand");

    return true;
}

//! @brief 更新
void CloseEnemy::Update()
{
    Super::Update();
}

//! @brief 衝突時の処理
void CloseEnemy::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    // 当たりの情報を渡す
    if(auto action = GetComponent<CloseEnemyAction>())
    {
        action->OnHit(hit_info);
    }

    if(auto knockback = GetComponent<Knockback>())
    {
        knockback->IsHitGround(hit_info);
    }
    Super::OnHit(hit_info);
}

//! @brief 終了
void CloseEnemy::Exit()
{
    Super::Exit();

    ClearEnemyLockPoint();
    if(weapon_sword_)
    {
        weapon_sword_->RemoveAllComponents();
        Scene::Object::Release(weapon_sword_);
        weapon_sword_.reset();
    }
}

//! @brief ロックポイント作成
EnemyLockPointPtr CloseEnemy::CreateEnemyLockPoint()
{
    // ロックポイントを設定
    if(!lock_point_)
    {
        lock_point_ = Scene::Object::Create<EnemyLockPoint>("EnemyLockPoint");
        lock_point_->AddComponent<ComponentAttachModel>()->SetAttachObject(GetName(), "mixamorig:Spine");
        lock_point_->SetOwner(std::dynamic_pointer_cast<EnemyBase>(shared_from_this()));
    }
    return lock_point_;
}

//! @brief ロックポイントを作成せずにロック座標を取得
//! @return ロックポイントのワールド座標
float3 CloseEnemy::GetVirutalLockPointPos()
{
    if(auto model = GetComponent<ComponentModel>())
    {
        return model->GetNodePosition("mixamorig:Spine");
    }
    return GetTranslate();
}

//! @brief オブジェクト情報リセット
void CloseEnemy::ResetObjectInfo()
{
    Super::ResetObjectInfo();
    if(auto hp = GetComponent<ComponentHP>())
    {
        hp->SetMaxHP(MAX_HP_);
        hp->SetHP(MAX_HP_);
    }
    if(auto mental_manager = Scene::Object::Get<EnemyMentalManager>())
    {
        mental_manager->RegisterEnemyList(std::dynamic_pointer_cast<EnemyBase>(shared_from_this()));
    }
    if(auto action = GetComponent<CloseEnemyAction>())
    {
        action->SetAction(std::make_shared<CloseEnemyFindPlayer>());
    }
}

//===========================================================================
//! @file FarEnemy.cpp
//! @brief 遠距離敵
//===========================================================================
#include "FarEnemy.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/Enemy/FarEnemy/FarEnemyAnimationController.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/WeaponAttack.h>
#include <System/Component/ComponentAttachModel.h>
#include <Game/Component/Sound.h>
#include <Game/Component/ReciprocalVelocityObstacles.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/Navmesh.h>
#include <Game/Component/Enemy/Knockback.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyAction.h>
#include <Game/Component/Enemy/AttackParam.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyIdle.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyFindPlayer.h>
#include <Game/Object/Manager/EnemyMentalManager.h>

//! @brief 初期化
//! @return 初期化済み
bool FarEnemy::Init()
{
    Super::Init();

    SetName("FarEnemy");

    // モデルコンポーネントを追加
    auto model = AddComponent<ComponentModel>("data/Game/Model/FarEnemy/Vanguard By T. Choonyung.mv1");   // モデルコンポーネント

    // スケールを設定
    SetScaleAxisXYZ({1.3f, 1.3f, 1.3f});

    // コリジョン情報を設定
    collision_info_.radius_ = 2.3f;
    collision_info_.height_ = 13.5f;
    collision_info_.mass_   = 10.0f;
    // コリジョンコンポーネントを追加
    ResetCollision();

    // 行動コンポーネントを追加
    AddComponent<FarEnemyAction>();
    // HPコンポーネントを追加
    AddComponent<ComponentHP>()->SetMaxHP(MAX_HP_)->SetHP(MAX_HP_);
    // 音の読み込み
    AddComponent<Sound>()->Load({
        {"hit",   "data/Game/SE/Enemy/Far/「うっ！」.mp3",     128.0f, 0.7f},
        {"death", "data/Game/SE/Enemy/Far/「ぐああっ！」.mp3", 128.0f, 1.0f},
    });

    AddComponent<FarEnemyAnimationController>();
    AddComponent<EnemyMove>();
    AddComponent<Navmesh>();
    AddComponent<RVO>();
    AddComponent<AttackParam>();

    gun_ = Scene::Object::Create<EnemyGun>();
    gun_->SetOwner(shared_from_this());
    gun_->AttachModel("mixamorig:RightHand");
    return true;
}

//! @brief 更新
void FarEnemy::Update()
{
    Super::Update();
}

//! @brief 衝突時の処理
void FarEnemy::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    if(auto action = GetComponent<FarEnemyAction>())
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
void FarEnemy::Exit()
{
    Super::Exit();

    ClearEnemyLockPoint();
}

//! @brief ロックオンポイントを作成
//! @return ロックオンポイント
EnemyLockPointPtr FarEnemy::CreateEnemyLockPoint()
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
float3 FarEnemy::GetVirutalLockPointPos()
{
    if(auto model = GetComponent<ComponentModel>())
    {
        return model->GetNodePosition("mixamorig:Spine");
    }
    return GetTranslate();
}

//! @brief 武器を取得
//! @return 武器
EnemyGunPtr FarEnemy::GetWeaponPtr() const
{
    return gun_;
}

//! @brief オブジェクト情報をリセット
void FarEnemy::ResetObjectInfo()
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
    if(auto action = GetComponent<FarEnemyAction>())
    {
        action->SetAction(std::make_shared<FarEnemyFindPlayer>());
    }
}

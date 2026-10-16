//===========================================================================
//! @file Boss.cpp
//! @brief ボス
//===========================================================================
#include "Boss.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/WeaponAttack.h>
#include <System/Component/ComponentAttachModel.h>
#include <Game/Component/Sound.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/Enemy/Knockback.h>
#include <Game/Component/Enemy/Boss/BossAnimationController.h>
#include <Game/Component/Enemy/AttackParam.h>
#include <Game/Component/Enemy/Boss/Action/BossAction.h>
#include <Game/Component/Enemy/Boss/BossActionManager.h>
#include <System/Component/ComponentTargetTracking.h>
#include <Game/Component/UI/BossHPUI.h>

//! @brief 初期化
//! @return 初期化済み
bool Boss::Init()
{
    Super::Init();

    SetName("Boss");

    // モデルコンポーネントを追加
    auto model = AddComponent<ComponentModel>("data/Game/Model/Boss/Nightshade J Friedrich.mv1");   // モデルコンポーネント

    // スケールを設定
    SetScaleAxisXYZ({1.2f, 1.2f, 1.2f});

    // コリジョンコンポーネントを追加
    AddComponent<ComponentCollisionCapsule>()
        ->SetRadius(3.0f)
        ->SetHeight(20.0f)
        ->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY)
        ->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::GROUND | (u32)ComponentCollision::CollisionGroup::PLAYER |
                               (u32)ComponentCollision::CollisionGroup::WEAPON | (u32)ComponentCollision::CollisionGroup::DODGE |
                               (u32)ComponentCollision::CollisionGroup::ULTIMATE_ATTACK | (u32)ComponentCollision::CollisionGroup::ENEMY)
        ->SetMass(50.0f)
        ->UseGravity();

    // HPコンポーネントを追加
    AddComponent<ComponentHP>()->SetMaxHP(MAX_HP_)->SetHP(MAX_HP_);

    AddComponent<BossAnimationController>();
    AddComponent<EnemyMove>()->SetSpeed(0.4f);
    AddComponent<AttackParam>();
    AddComponent<BossAction>();
    AddComponent<BossActionManager>();

    // ボスHP UIコンポーネントを追加
    auto boss_hp_ui = AddComponent<BossHPUI>();
    boss_hp_ui->StartOpenAnimation();

    AddComponent<Sound>()->Load({
        {"approach clap",  "data/Game/SE/Boss/hakusyuone.mp3",                 256.0f, 0.7f},
        {"approach dash",  "data/Game/SE/Boss/着火1.mp3",                      256.0f, 0.7f},
        {"walk",           "data/Game/SE/Boss/アスファルトの上を歩く1.mp3",    128.0f, 0.3f},
        {"attack sound1",  "data/Game/SE/Boss/08_cool_attack.wav",             128.0f, 0.7f},
        {"attack sound2",  "data/Game/SE/Boss/07_cool_attack.wav",             128.0f, 0.7f},
        {"attack success", "data/Game/SE/Boss/打撃3.mp3",                      128.0f, 0.7f},
        {"dash sound",     "data/Game/SE/Boss/09_cool_attack.wav",             128.0f, 1.0f},
        {"dash attack",    "data/Game/SE/Boss/打撃4.mp3",                      512.0f, 1.0f},
        {"blast",          "data/Game/SE/Boss/狙撃銃発射.mp3",                 256.0f, 1.0f},
        {"hit",            "data/Game/SE/Boss/14_cool_damage.wav",             128.0f, 0.7f},
        {"victory",        "data/Game/SE/Boss/31_cool_win.wav",                256.0f, 1.0f},
        {"death",          "data/Game/SE/Boss/「くっ、こんなはずでは……」.mp3", 256.0f, 1.0f},
    });

    // 頭をプレイヤーに向ける
    auto target_tracking = AddComponent<ComponentTargetTracking>();
    target_tracking->SetTrackingNode("mixamorig:Head");
    target_tracking->SetTargetObjectPtr("Player");
    target_tracking->SetTrackingLimitLeftRight({0.0f, 70.0f});
    target_tracking->SetTrackingLimitUpDown({0.0f, 0.0f});
    target_tracking->SetTrackingFrameLimit(3.0f);

    lock_point_offset_ = model->GetNodePosition("mixamorig:Spine1", true);

    return true;
}

//! @brief 更新
void Boss::Update()
{
    Super::Update();
    if(auto se = GetComponent<Sound>())
    {
        // プレイヤーのHPが0以下になったら勝利音を再生
        if(auto player = Scene::Object::Get<Player>())
        {
            auto hp = player->GetComponent<ComponentHP>();
            if(hp->GetHP() <= 0)
            {
                se->PlayOnce("victory");
            }
        }
    }
    if(lock_point_)
    {
        lock_point_->SetTranslate(GetVirutalLockPointPos());
    }
}

//! @brief 衝突時の処理
void Boss::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    // 当たりの情報を渡す
    if(auto action = GetComponent<BossAction>())
    {
        action->OnHit(hit_info);
    }

    Super::OnHit(hit_info);
}

//! @brief 終了
void Boss::Exit()
{
    Super::Exit();

    ClearEnemyLockPoint();
}

//! @brief 自分のロックオンポイントを作成する
EnemyLockPointPtr Boss::CreateEnemyLockPoint()
{
    // ロックポイントを設定
    if(!lock_point_)
    {
        lock_point_ = Scene::Object::Create<EnemyLockPoint>("EnemyLockPoint");
        lock_point_->SetOwner(std::dynamic_pointer_cast<EnemyBase>(shared_from_this()));
    }
    return lock_point_;
}

//! @brief ロックポイントを作成せずにロック座標を取得
//! @return ロックポイントの座標
float3 Boss::GetVirutalLockPointPos()
{
    return GetTranslate() + lock_point_offset_ * GetScaleAxisXYZ();
}

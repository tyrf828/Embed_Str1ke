//===========================================================================
//! @file EnemySword.cpp
//! @brief 敵の武器（剣）
//===========================================================================
#include "EnemySword.h"
#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentModel.h>
#include <Game/Component/Enemy/AttackParam.h>
#include <Game/Object/Effect/EnemyHitEffect.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
//! @return 初期化済み
bool EnemySword::Init()
{
    Super::Init();

    SetName("EnemySword");

    AddComponent<Sound>()->Load({
        {"attack", "data/Game/SE/Enemy/Close/剣で斬る2.mp3", 128.0f, 0.7f},
    });

    // モデルを設定
    AddComponent<ComponentModel>("data/Game/Model/EnemyWeapon/Sword/Sword2_Red.mv1");
    AddComponent<ComponentAttachModel>();
    AddComponent<AttackParam>();

    return true;
}
//! @brief 更新
void EnemySword::Update()
{
    Super::Update();
    if(owner_.lock())
    {
        // 攻撃力をオーナーに同期
        if(auto attack = GetComponent<AttackParam>())
        {
            if(auto owner_attack = owner_.lock()->GetComponent<AttackParam>())
            {
                attack->SetAttack(owner_attack->GetAttack());
            }
        }
    }
}

//! @brief 衝突時の処理
void EnemySword::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    if(static_cast<u32>(hit_info.hit_collision_->GetCollisionGroup()) & static_cast<u32>(ComponentCollision::CollisionGroup::PLAYER))
    {
        // ヒットエフェクト生成
        Scene::Object::Create<EnemyHitEffect>()->SetTranslate(hit_info.hit_position_);
        // 攻撃判定削除
        RemoveCollision();
        // 音の再生
        if(auto sound = GetComponent<Sound>())
        {
            sound->PlayNoSame("attack");
        }
    }
    Super::OnHit(hit_info);
}
//! @brief 更新後
void EnemySword::PostUpdate()
{
    Super::PostUpdate();
}

//! @brief モデルのアタッチ
//! @param node ノード名
void EnemySword::AttachModel(std::string_view node)
{
    if(auto attach = GetComponent<ComponentAttachModel>())
    {
        attach->SetAttachObject(owner_.lock(), node);
        attach->SetAttachRotate(float3(0.0f, 0.0f, -90.0f));
        attach->SetAttachOffset(float3(0.0f, 13.0f, -2.5f));
    }
    if(auto model = GetComponent<ComponentModel>())
    {
        model->SetScaleAxisXYZ({0.5f, 0.5f, 0.5f});
    }
}

//! @brief 攻撃判定の追加
void EnemySword::AddCollision()
{
    if(!GetComponent<ComponentCollisionCapsule>())
    {
        auto col = AddComponent<ComponentCollisionCapsule>();
        col->SetTranslate(float3(0.0f, -15.0f, 0.0f));
        col->SetRadius(7.5f);
        col->SetHeight(110.0f);
        col->SetCollisionGroup(ComponentCollision::CollisionGroup::ETC);
        col->SetOverlapCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER | (u32)ComponentCollision::CollisionGroup::GROUND |
                                      (u32)ComponentCollision::CollisionGroup::WEAPON | (u32)ComponentCollision::CollisionGroup::ETC |
                                      (u32)ComponentCollision::CollisionGroup::ENEMY_WEAPON | (u32)ComponentCollision::CollisionGroup::ENEMY);
    }
}

//! @brief 攻撃判定の削除
void EnemySword::RemoveCollision()
{
    if(auto col = GetComponent<ComponentCollisionCapsule>())
    {
        RemoveComponent(col);
    }
}

//! @brief 攻撃判定の有効無効設定
void EnemySword::SetHitEnable(bool enable)
{
    if(auto col = GetComponent<ComponentCollisionCapsule>())
    {
        if(enable)
        {
            col->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY_WEAPON);
            return;
        }
        col->SetCollisionGroup(ComponentCollision::CollisionGroup::ETC);
    }
}

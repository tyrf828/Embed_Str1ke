//===========================================================================
//! @file EnemyGun.cpp
//! @brief 敵の武器
//===========================================================================
#include "EnemyGun.h"
#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentModel.h>
#include <Game/Component/Enemy/AttackParam.h>
#include <Game/Object/Effect/ShootLaserEffect.h>
#include <Game/Object/Enemy/FarEnemy/GunHitCheck.h>
#include <System/Component/ComponentEffect.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyAttack.h>
#include <Game/Object/Effect/EnemyHitEffect.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
//! @return 初期化済み
bool EnemyGun::Init()
{
    Super::Init();

    SetName("EnemyGun");

    // モデルを設定
    AddComponent<ComponentModel>("data/Game/Model/EnemyWeapon/Gun/Photon_Blaster.mv1");
    AddComponent<ComponentAttachModel>();
    AddComponent<AttackParam>();

    AddComponent<Sound>()->Load({
        {"charge", "data/Game/SE/Enemy/Far/ビーム砲チャージ.mp3", 256.0f, 1.0f},
        {"attack", "data/Game/SE/Enemy/Far/レーザー.mp3",         256.0f, 0.7f},
    });

    return true;
}
//! @brief 更新
void EnemyGun::Update()
{
    Super::Update();
    if(owner_.lock())
    {
        if(auto attack = GetComponent<AttackParam>())
        {
            // 攻撃力をオーナーから取得して設定
            if(auto owner_attack = owner_.lock()->GetComponent<AttackParam>())
            {
                attack->SetAttack(owner_attack->GetAttack());
            }
        }
    }
    if(auto effect = effect_.lock())
    {
        // エフェクトの位置と向きを更新
        effect->SetTranslate(GetBarrelPos());
        effect->SetRotationToVector(-owner_.lock()->GetPlaneDir());

        if(auto effect_component = effect->GetComponent<ComponentEffect>())
        {
            // エフェクトの攻撃判定の削除処理
            static const float END_TIME = 1.8f;
            if(effect_component->GetEffectTime() > END_TIME * effect_component->GetPlaySpeed())
            {
                RemoveHit();
                return;
            }

            // エフェクトの攻撃判定の設定処理
            static const float SHOOT_TIME = 0.65f;
            if(effect_component->GetEffectTime() > SHOOT_TIME * effect_component->GetPlaySpeed() && !is_shoot_)
            {
                SetHitCollision();
                is_shoot_ = true;
                if(auto se = GetComponent<Sound>())
                {
                    se->Stop("charge");
                }
            }
        }
    }

    if(gun_hit_.lock())
    {
        if(owner_.lock())
        {
            // 攻撃判定を少し下げる
            const float OFFSET_Y = (GetBarrelPos().y - owner_.lock()->GetTranslate().y) * 0.5f;
            gun_hit_.lock()->SetTranslate(float3{owner_.lock()->GetTranslate().x, GetBarrelPos().y - OFFSET_Y, owner_.lock()->GetTranslate().z});
        }
        else
        {
            gun_hit_.lock()->SetTranslate(GetBarrelPos());
        }

        gun_hit_.lock()->SetRotationToVector(-owner_.lock()->GetPlaneDir());
        // 攻撃距離を更新
        current_attack_length_ += FarEnemyAttack::GetAttackDistance() * GetDeltaTime() * 2.0f;
        current_attack_length_  = std::min(current_attack_length_, FarEnemyAttack::GetAttackDistance());
        HitLength(current_attack_length_);
    }
}

//! @brief モデルをアタッチ
//! @param node アタッチするノード名
void EnemyGun::AttachModel(std::string_view node)
{
    if(auto attach = GetComponent<ComponentAttachModel>())
    {
        attach->SetAttachObject(owner_.lock(), node);
        attach->SetAttachRotate(float3(65.0f, 0.0f, -80.0f));
        attach->SetAttachOffset(float3(5.0f, 23.0f, -10.0f));
    }
    if(auto model = GetComponent<ComponentModel>())
    {
        model->SetScaleAxisXYZ({0.2f, 0.2f, 0.2f});
    }
}

//! @brief 攻撃判定を設定
void EnemyGun::SetHit()
{
    if(effect_.lock())
    {
        return;
    }
    effect_ = Scene::Object::Create<ShootLaserEffect>();
    if(auto se = GetComponent<Sound>())
    {
        se->PlayNoSame("charge");
    }
}

//! @brief 攻撃判定のコリジョンを設定
void EnemyGun::SetHitCollision()
{
    if(!gun_hit_.expired())
    {
        return;
    }
    gun_hit_ = Scene::Object::Create<GunHitCheck>();
    gun_hit_.lock()->SetOwner(shared_from_this());
    if(auto se = GetComponent<Sound>())
    {
        se->PlayNoSame("attack");
    }
}

//! @brief 攻撃判定を削除
void EnemyGun::RemoveHit()
{
    Scene::Object::Release(gun_hit_.lock());
    Scene::Object::Release(effect_.lock());
    is_shoot_ = false;
    gun_hit_.reset();
    effect_.reset();
    current_attack_length_ = 0.0f;
}

//! @brief 攻撃距離を設定
//! @param length 距離
void EnemyGun::HitLength(float length)
{
    if(auto gun_hit = gun_hit_.lock())
    {
        gun_hit->SetAttackLength(length);
    }
}

//! @brief 銃口の位置を取得
//! @param use_local ローカル座標を使用するか
//! @return 銃口の位置
float3 EnemyGun::GetBarrelPos(bool use_local) const
{
    if(auto model = GetComponent<ComponentModel>())
    {
        return model->GetNodePosition("GunBarrel", use_local);
    }
    return float3{0.0f, 0.0f, 0.0f};
}

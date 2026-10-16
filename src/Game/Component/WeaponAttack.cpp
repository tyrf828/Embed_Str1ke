//===========================================================================
//! @file WeaponAttack.cpp
//! @brief 武器攻撃の管理
//===========================================================================
#include "WeaponAttack.h"
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentModel.h>
#include <Game/Object/Effect/HitEffect.h>
#include <Game/Object/Enemy/EnemyBase.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/Sound.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Component/Weapon/AttachWeaponRemain.h>
#include <Game/Component/Player/UltimateGauge.h>
#include <Game/Object/Effect/DamageText.h>
#include <Game/Component/Weapon/AttachWeaponParts.h>

//! @brief 初期化
void WeaponAttack::Init()
{
    Super::Init();
}

//! @brief 更新
void WeaponAttack::Update()
{
    Super::Update();

    // パーツ数に応じて攻撃力アップ
    final_atk_ = GetBaseATK();
    if(auto parts = GetOwner()->GetComponent<AttachWeaponParts>())
    {
        for(int i = 0; i < parts->GetParts().size(); i++)
        {
            final_atk_ = (int)(std::round(final_atk_ * ATTACK_MULTIPLY_));
        }
    }
}

//! @brief 更新後処理
void WeaponAttack::LateUpdate()
{
    Super::LateUpdate();
}

//! @brief GUI表示処理
void WeaponAttack::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("WeaponAttack"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::InputInt(UNIQUE_TEXT(u8"攻撃力"), &attack_point_);
            ImGui::InputInt(UNIQUE_TEXT(u8"最終攻撃力"), &final_atk_);
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//! @brief 攻撃判定を消す
void WeaponAttack::SetNoHit()
{
    // 当たり判定を変更
    if(auto col = GetOwner()->GetComponent<ComponentCollisionCapsule>())
    {
        col->SetCollisionGroup(ComponentCollision::CollisionGroup::ETC);
    }
}

//! @brief 攻撃判定付ける
void WeaponAttack::SetHit()
{
    // 当たり判定を変更
    if(auto col = GetOwner()->GetComponent<ComponentCollisionCapsule>())
    {
        col->SetCollisionGroup(ComponentCollision::CollisionGroup::WEAPON);
    }
}

//! @brief 攻撃力を設定
//! @param atk_point 設定する攻撃力
void WeaponAttack::SetBaseATK(int atk_point)
{
    attack_point_ = atk_point;
}

//! @brief 攻撃力を取得
//! @return 攻撃力
int WeaponAttack::GetBaseATK() const
{
    return attack_point_;
}

//! @brief 最終攻撃力を取得
//! @return 最終攻撃力
int WeaponAttack::GetATK() const
{
    return final_atk_;
}

//! @brief 敵にヒットした時の処理
//! @param hit_info ヒット情報
void WeaponAttack::HitEnemy(const ComponentCollision::HitInfo& hit_info)
{
    HitFeedback(hit_info.hit_position_);

    // ダメージポップアップ表示
    {
        auto text = Scene::Object::Create<DamageText>();
        text->Set(final_atk_, hit_info.hit_position_ + float3(0.0f, 1.0f, 0.0f));
    }

    // アタッチした武器の残数を減らす
    if(auto remain = GetOwner()->GetComponent<AttachWeaponRemain>())
    {
        remain->SubRemain(1);
    }

    // 必殺技ゲージを増加させる
    if(auto ult = Scene::Object::Get<Player>()->GetComponent<UltimateGauge>())
    {
        ult->AddEnergy(int(final_atk_ * 0.5f));
    }
}

//! @brief ヒットフィードバック
//! @param effect_position エフェクト発生位置
void WeaponAttack::HitFeedback(const float3& effect_position)
{
    // ヒットエフェクトを生成
    Scene::Object::Create<HitEffect>()->SetTranslate(effect_position);

    // 音の再生
    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        se->Play("hit target");
    }
    // カメラを揺らす
    if(auto camera = Scene::Object::Get<Camera>())
    {
        // カメラを揺らす
        camera->ShakeCamera(0.1f, 1.0f);
    }
    // ヒットストップ
    if(auto player = Scene::Object::Get<Player>())
    {
        player->GetComponent<ComponentModel>()->SetHitStop(0.2f);
    }
}

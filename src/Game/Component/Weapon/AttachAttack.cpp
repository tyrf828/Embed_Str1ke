//===========================================================================
//! @file AttachAttack.cpp
//! @brief アタッチ攻撃コンポーネント
//===========================================================================
#include "AttachAttack.h"
#include <Game/Component/Weapon/AttachWeapon.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <Game/Object/Weapon/Stick.h>
#include <Game/Component/Weapon/AttachWeaponParts.h>

//! @brief 初期化
void AttachAttack::Init()
{
    Super::Init();
}

//! @brief 更新
void AttachAttack::Update()
{
    Super::Update();
}

//! @brief 衝突時の処理
//! @param hit_info 衝突したオブジェクトの情報
void AttachAttack::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    // アイテムとの衝突判定
    if(static_cast<u32>(hit_info.hit_collision_->GetCollisionGroup()) & static_cast<u32>(ComponentCollision::CollisionGroup::ITEM))
    {
        // まだ武器としてアタッチされていない場合
        if(!hit_info.hit_collision_->GetOwner()->GetComponent<AttachWeapon>())
        {
            // 空きフレームがない場合は、登録を行わない
            if(!AttachWeapon::CanAttach(hit_info.hit_position_, GetOwnerPtr(), hit_info.hit_collision_->GetOwnerPtr()))
            {
                return;
            }
            if(auto model = hit_info.hit_collision_->GetOwner()->GetComponent<ComponentModel>())
            {
                float3 vector = hit_info.hit_collision_->GetOwner()->GetTranslate() - hit_info.hit_position_;
            }

            // アタッチ武器コンポーネントを追加と設定
            hit_info.hit_collision_->GetOwner()->AddComponent<AttachWeapon>()->SetWeapon(hit_info.hit_position_, GetOwnerPtr());
        }

        // スティックにパーツ追加
        if(auto stick = Scene::Object::Get<Stick>())
        {
            stick->GetComponent<AttachWeaponParts>()->AddPart(hit_info.hit_collision_->GetOwnerPtr());
        }
    }
}

//! @brief 描画
void AttachAttack::LateDraw()
{
    Super::LateDraw();
}

//! @brief GUI表示処理
void AttachAttack::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("ComponentAttachAttack"))
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

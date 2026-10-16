//===========================================================================
//! @file AttachWeaponParts.cpp
//! @brief 武器の物件
//===========================================================================
#include "AttachWeaponParts.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionBox.h>
#include <Game/Component/Weapon/AttachAttack.h>
#include <Game/Component/Weapon/AttachWeapon.h>
#include <Game/Component/Weapon/AttachWeaponRemain.h>
#include <Game/Component/WeaponAttack.h>

//! @brief 初期化
void AttachWeaponParts::Init()
{
    Super::Init();
}

//! @brief 更新
void AttachWeaponParts::Update()
{
    Super::Update();

    auto model       = GetOwner()->GetComponent<ComponentModel>();
    int stick_handle = model->GetModelHandle();

    // パーツがない場合は自身の当たり判定を有効化、ある場合は無効化
    if(weapon_parts.size() == 0)
    {
        if(auto col = GetOwner()->GetComponent<ComponentCollisionCapsule>())
        {
            col->SetCollisionStatus(ComponentCollision::CollisionBit::DisableHit, false);
        }
    }
    else
    {
        if(auto col = GetOwner()->GetComponent<ComponentCollisionCapsule>())
        {
            col->SetCollisionStatus(ComponentCollision::CollisionBit::DisableHit, true);
        }
    }

    // 各パーツの更新処理
    for(auto part : weapon_parts)
    {
        auto col = part->GetComponent<ComponentCollisionCapsule>();
        if(!col)
        {
            part->GetComponent<ComponentCollisionBox>();
        }
        if(!col)
        {
            return;
        }
        // コリジョングループの同期
        col->SetCollisionGroup(GetOwner()->GetComponent<ComponentCollisionCapsule>()->GetCollisionGroup());

        // アタッチ攻撃コンポーネントの同期
        if(GetOwner()->GetComponent<AttachAttack>())
        {
            if(!part->GetComponent<AttachAttack>())
            {
                part->AddComponent<AttachAttack>();
            }
        }
        else
        {
            if(part->GetComponent<AttachAttack>())
            {
                part->RemoveComponent<AttachAttack>();
            }
        }

        // 攻撃力の同期
        if(!part->GetComponent<WeaponAttack>())
        {
            part->AddComponent<WeaponAttack>();
        }
        else
        {
            part->GetComponent<WeaponAttack>()->SetBaseATK(GetOwner()->GetComponent<WeaponAttack>()->GetATK());
        }

        auto part_model = part->GetComponent<ComponentModel>();

        // 透明度処理
        {
            auto mesh_num = MV1GetMeshNum(part_model->GetModel());
            float opacity = MV1GetMeshOpacityRate(stick_handle, 0);
            for(int i = 0; i < mesh_num; i++)
            {
                MV1SetMeshOpacityRate(part_model->GetModel(), i, opacity);
            }

            // 透明度が必要な場合はフォワードレンダリングに切り替え
            if(model->IsUseForwardRendering())
            {
                part_model->UseForwardRendering(true);

                for(int i = 0; i < mesh_num; i++)
                {
                    MV1SetMeshDrawBlendMode(part_model->GetModel(), i, DX_BLENDMODE_ALPHA);
                }
            }
            else
            {
                // ディファードに戻す
                part_model->UseForwardRendering(false);
            }
        }
    }
}

//! @brief GUI表示処理
void AttachWeaponParts::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("AttachWeaponParts"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }

            for(auto part : weapon_parts)
            {
                ImGui::Text(part->GetName().data());
            }

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//! @brief 終了
void AttachWeaponParts::Exit()
{
    Super::Exit();

    weapon_parts.clear();
}

//! @brief パーツを追加
//! @param obj パーツオブジェクト
void AttachWeaponParts::AddPart(ObjectPtr obj)
{
    // 同じなのかチェック
    for(auto part : weapon_parts)
    {
        if(part == obj)
        {
            return;
        }
    }

    weapon_parts.push_back(obj);
}

//! @brief 武器のパーツと以降のを削除
//! @param obj パーツオブジェクト
void AttachWeaponParts::RemoveParts(ObjectPtr obj)
{
    int index = 0;
    for(auto part : weapon_parts)
    {
        if(part == obj)
        {
            break;
        }
        index++;
    }

    // このパーツ以降のパーツも削除
    weapon_parts.erase(weapon_parts.begin() + index, weapon_parts.end());
}

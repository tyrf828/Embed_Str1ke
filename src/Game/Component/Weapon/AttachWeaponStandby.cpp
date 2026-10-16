//===========================================================================
//! @file AttachWeaponStandby.cpp
//! @brief 武器待機
//===========================================================================
#include "AttachWeaponStandby.h"
#include <Game/Object/Manager/SpawnManager.h>
#include <Game/Component/Weapon/AttachAttack.h>
#include <Game/Component/Weapon/AttachWeapon.h>
#include <Game/Component/Weapon/AttachWeaponRemain.h>
#include <Game/Component/Weapon/AttachWeaponParts.h>
#include <Game/Object/Weapon/Stick.h>

//! @brief 初期化
void AttachWeaponStandby::Init()
{
    Super::Init();
    // コリジョン削除
    if(GetOwner()->GetComponent<ComponentCollision>())
    {
        GetOwner()->RemoveComponent<ComponentCollision>();
    }
    // 位置リセット
    GetOwner()->SetTranslate(float3(100.0f, 10.0f, 100.0f));
    GetOwner()->SetRotationAxisXYZ(float3(0.0f, 0.0f, 0.0f));

    // スポーンプールに追加
    auto spawn_manager = Scene::Object::Get<SpawnManager>("ItemSpawnManager");
    if(!spawn_manager)
    {
        spawn_manager = Scene::Object::Get<SpawnManager>();
    }
    if(spawn_manager)
    {
        spawn_manager->AddSpawnPool(GetOwnerPtr());
    }

    // スティックのパーツリストから削除
    if(auto stick = Scene::Object::Get<Stick>())
    {
        stick->GetComponent<AttachWeaponParts>()->RemovePart(GetOwnerPtr());
    }
}

//! @brief 更新
void AttachWeaponStandby::Update()
{
    Super::Update();
    // 処理負荷軽減のため更新、描画を停止
    GetOwner()->SetStatus(Object::StatusBit::NoUpdate, true);
    GetOwner()->SetStatus(Object::StatusBit::NoDraw, true);
}

//! @brief 終了
void AttachWeaponStandby::Exit()
{
    Super::Exit();
    // 更新、描画を再開
    GetOwner()->SetStatus(Object::StatusBit::NoUpdate, false);
    GetOwner()->SetStatus(Object::StatusBit::NoDraw, false);
}

//! @brief GUI表示処理
void AttachWeaponStandby::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("AttachWeaponStandby"))
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

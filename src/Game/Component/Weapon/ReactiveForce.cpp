//===========================================================================
//! @file ReactiveForce.cpp
//! @brief 武器反動力
//===========================================================================
#include "ReactiveForce.h"
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/Weapon/AttachWeaponStandby.h>
#include <Game/Object/Manager/SpawnManager.h>
#include <Game/Object/Weapon/Stick.h>
#include <Game/Component/Weapon/AttachWeaponParts.h>

//! @brief 初期化
void ReactiveForce::Init()
{
    Super::Init();

    // コリジョン設定
    if(auto col = GetOwner()->GetComponent<ComponentCollisionCapsule>())
    {
        col->SetCollisionStatus(ComponentCollision::CollisionBit::DisableHit, true);
        col->UseGravity(true);
    }
    // 回転軸を記録
    force_axis_ = GetOwner()->GetRotationAxisXYZ();
}

//! @brief 更新
void ReactiveForce::Update()
{
    Super::Update();

    life_time_ += GetDeltaTime();

    // Yは上向きに固定
    dir_.y = abs(dir_.y);

    // 破棄条件でなければ回転を更新
    if(GetOwner()->GetTranslate().y > -100.0f && life_time_ <= LIFE_TIME_MAX_)
    {
        // 法線
        float3 ground_normal = float3(0.0f, 1.0f, 0.0f);
        // 回転軸
        float3 rotation_axis  = normalize(cross(dir_, ground_normal));
        force_axis_          += rotation_axis * rot_power_;
        // 回転角度更新
        GetOwner()->SetRotationAxisXYZ(force_axis_);
    }
    else
    {
        // 一定以下の高さになったら削除
        GetOwner()->RemoveComponent(shared_from_this());
    }
}

//! @brief 描画
void ReactiveForce::LateDraw()
{
    Super::LateDraw();
}

//! @brief GUI表示処理
void ReactiveForce::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("ReactiveForce"))
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

//! @brief 終了
void ReactiveForce::Exit()
{
    Super::Exit();
    // 待機状態へ移行
    if(!GetOwner()->GetComponent<AttachWeaponStandby>())
    {
        GetOwner()->AddComponent<AttachWeaponStandby>();
    }

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
}

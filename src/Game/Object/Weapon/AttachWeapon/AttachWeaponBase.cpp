//===========================================================================
//! @file AttachWeaponBase.h
//! @brief プレイヤーの武器基底クラス
//===========================================================================
#include "AttachWeaponBase.h"
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/Weapon/AttachAttack.h>
#include <Game/Component/Weapon/AttachWeapon.h>
#include <Game/Component/Weapon/AttachWeaponStandby.h>
#include <Game/Component/OutLine.h>
#include <Game/Object/Player/Player.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Component/Weapon/ReactiveForce.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
//! @return 初期化済み
bool AttachWeaponBase::Init()
{
    Super::Init();

    // 武器待機コンポーネントを追加
    AddComponent<AttachWeaponStandby>();

    // オブジェクト情報のリセット
    ResetObjectInfo();

    // アタッチ提示画像の読み込み
    attach_key_ghandle_[0] = LoadGraph("data/Game/GHandle/Keyboard & Mouse/Default/mouse_right.png");
    attach_key_ghandle_[1] = LoadGraph("data/Game/GHandle/Xbox Series/Default/xbox_button_color_b.png");

    return true;
}

//! @brief 更新
void AttachWeaponBase::Update()
{
    Super::Update();
    // アタッチ武器コンポーネントが存在する場合、輪郭線を削除
    if(GetComponent<OutLine>())
    {
        if(GetComponent<AttachWeapon>())
        {
            RemoveComponent<OutLine>();
        }
    }
}

//! @brief 遅い描画
void AttachWeaponBase::LateDraw()
{
    Super::LateDraw();
    auto player = Scene::Object::Get<Player>();
    auto camera = Scene::Object::Get<Camera>();
    if(!player || !camera)
    {
        return;
    }
    float distance = length(player->GetTranslate() - GetTranslate());
    // 一定距離以上離れているか、カメラの視野内に存在しない場合は描画しない
    if(distance > 50.0f || !camera->IsInCameraView(GetTranslate()))
    {
        return;
    }
    auto col           = GetComponent<ComponentCollisionCapsule>();
    auto attach_weapon = GetComponent<AttachWeapon>();
    auto rea_force     = GetComponent<ReactiveForce>();
    if(!col || attach_weapon || rea_force)
    {
        return;
    }
    float3 col_offset = col->GetTranslate();
    VECTOR pos_2d     = ConvWorldPosToScreenPos(cast(GetTranslate() + float3{0.0f, collision_data_.height_ * 1.3f, 0.0f} + col_offset));
    // アタッチ提示画像の描画
    if(GetConnecetdPadNum() == -1)
    {
        DrawRotaGraphF(pos_2d.x, pos_2d.y, 1.0f * SCALE_WINDOW, 0.0f, attach_key_ghandle_[0], TRUE);
    }
    else
    {
        DrawRotaGraphF(pos_2d.x, pos_2d.y, 1.0f * SCALE_WINDOW, 0.0f, attach_key_ghandle_[1], TRUE);
    }
}

//! @brief 衝突時の処理
//! @param hit_info 衝突したオブジェクトの情報
void AttachWeaponBase::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    // ヒット情報を渡す
    if(auto attach_attack = GetComponent<AttachAttack>())
    {
        attach_attack->OnHit(hit_info);
    }
    if(auto attach_weapon = GetComponent<AttachWeapon>())
    {
        attach_weapon->SetHitPosition(hit_info.hit_position_);
    }

    auto hit_group = hit_info.hit_collision_->GetCollisionGroup();
    auto my_group  = hit_info.collision_->GetCollisionGroup();
    // 敵に当たった場合、音を再生
    if((static_cast<u32>(hit_group) & static_cast<u32>(ComponentCollision::CollisionGroup::ENEMY)) &&
       (static_cast<u32>(my_group) & static_cast<u32>(ComponentCollision::CollisionGroup::WEAPON)))
    {
        if(auto se = GetComponent<Sound>())
        {
            se->PlayNoSame("hit");
        }
    }
    Super::OnHit(hit_info);
}

//! @brief オブジェクト情報のリセット
void AttachWeaponBase::ResetObjectInfo()
{
    Super::ResetObjectInfo();
    auto col = GetComponent<ComponentCollisionCapsule>();
    if(!col)
    {
        col = AddComponent<ComponentCollisionCapsule>();
    }
    col->SetCollisionGroup(ComponentCollision::CollisionGroup::ITEM)
        ->SetOverlapCollisionGroup((u32)ComponentCollision::CollisionGroup::CAMERA | (u32)ComponentCollision::CollisionGroup::WEAPON)
        ->SetRadius(collision_data_.radius_)
        ->SetHeight(collision_data_.height_)
        ->SetMass(collision_data_.mass_)
        ->UseGravity();

    if(GetComponent<AttachWeaponStandby>())
    {
        RemoveComponent<AttachWeaponStandby>();
    }
    if(!GetComponent<OutLine>())
    {
        AddComponent<OutLine>();
    }
    if(auto model = GetComponent<ComponentModel>())
    {
        model->UseForwardRendering(false);
    }
}

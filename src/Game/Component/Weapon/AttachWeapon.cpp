//===========================================================================
//! @file AttachWeapon.cpp
//! @brief 武器物件にアタッチ
//===========================================================================
#include <Game/Component/Weapon/AttachWeapon.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionBox.h>
#include <System/Component/ComponentAttachModel.h>
#include <System/Component/ComponentModel.h>
#include <Game/Object/Weapon/Stick.h>
#include <Game/Component/Weapon/AttachAttack.h>
#include <Game/Component/Weapon/AttachWeaponRemain.h>
#include <Game/Component/Weapon/ReactiveForce.h>
#include <Game/Component/Weapon/AttachWeaponParts.h>
#include <Game/Component/Weapon/AttachWeaponStandby.h>
#include <Game/Component/WeaponAttack.h>

//! @brief 初期化
void AttachWeapon::Init()
{
    Super::Init();

    // コリジョン設定
    if(auto col = GetOwner()->GetComponent<ComponentCollisionCapsule>())
    {
        col->SetCollisionGroup(ComponentCollision::CollisionGroup::ETC);
        col->SetOverlapCollisionGroup((u32)ComponentCollision::CollisionGroup::ENEMY | (u32)ComponentCollision::CollisionGroup::GROUND |
                                      (u32)ComponentCollision::CollisionGroup::WEAPON | (u32)ComponentCollision::CollisionGroup::ETC |
                                      (u32)ComponentCollision::CollisionGroup::ITEM);
        col->UseGravity(false);
    }
    if(auto col = GetOwner()->GetComponent<ComponentCollisionBox>())
    {
        col->SetCollisionGroup(ComponentCollision::CollisionGroup::ETC);
        col->SetOverlapCollisionGroup((u32)ComponentCollision::CollisionGroup::ENEMY | (u32)ComponentCollision::CollisionGroup::GROUND |
                                      (u32)ComponentCollision::CollisionGroup::WEAPON | (u32)ComponentCollision::CollisionGroup::ETC |
                                      (u32)ComponentCollision::CollisionGroup::ITEM);
        col->UseGravity(false);
    }
}

//! @brief 更新
void AttachWeapon::Update()
{
    Super::Update();
    auto stick  = Scene::Object::Get<Stick>();
    auto model  = GetOwner()->GetComponent<ComponentModel>();
    auto remain = GetOwner()->GetComponent<AttachWeaponRemain>();
    if(!remain || !model)
    {
        return;
    }

    // 武器の関節いる場合
    if(weapon_joint_)
    {
        if(Scene::Object::Get<Object>(weapon_joint_->GetName()))
        {
            if(auto weapon_model = weapon_object_->GetComponent<ComponentModel>())
            {
                // 関節の位置に合わせて自身の位置を調整
                weapon_frame_pos_ = weapon_model->GetNodePosition(weapon_frame_index_);

                matrix weapon_node_mat = weapon_model->GetNodeMatrix(weapon_frame_index_);
                float3 weapon_node_vec = normalize(weapon_node_mat.axisY());
                float3 vec             = GetOwner()->GetTranslate() - weapon_frame_pos_;

                GetOwner()->SetRotationAxisXYZ(weapon_joint_->GetRotationAxisXYZ());

                float3 axis_x       = weapon_joint_->GetVectorAxisX() * offset_.x;
                float3 axis_y       = weapon_joint_->GetVectorAxisY() * offset_.y;
                float3 axis_z       = weapon_joint_->GetVectorAxisZ() * offset_.z;
                float3 fixed_offset = axis_x + axis_y + axis_z;
                GetOwner()->SetTranslate(weapon_joint_->GetTranslate() - fixed_offset / weapon_joint_->GetScaleAxisXYZ());
            }
        }
    }

    // 耐久値がなくなった場合の処理
    if(remain->GetRemain() <= 0)
    {
        // 反動力を加えて吹き飛ばす
        if(!GetOwner()->GetComponent<ReactiveForce>())
        {
            auto rea_force = GetOwner()->AddComponent<ReactiveForce>();
            if(weapon_joint_)
            {
                rea_force->SetDirection(weapon_joint_->GetTranslate() - weapon_hit_pos_, 2.0f, 25.0f);
            }
        }
        // パーツ削除
        stick->GetComponent<AttachWeaponParts>()->RemovePart(GetOwnerPtr());

        GetOwner()->RemoveComponent(shared_from_this());
    }
    // 落下判定
    if(GetOwner()->GetTranslate().y <= -50.0f)
    {
        // 待機状態に移行
        if(!GetOwner()->GetComponent<AttachWeaponStandby>())
        {
            GetOwner()->AddComponent<AttachWeaponStandby>();
        }
        GetOwner()->RemoveComponent(shared_from_this());

        if(auto stick_part = stick->GetComponent<AttachWeaponParts>())
        {
            // パーツ削除
            stick_part->RemovePart(GetOwnerPtr());
        }
    }
    // 親オブジェクトの状態チェック
    if(weapon_joint_)
    {
        auto attach_model = weapon_joint_->GetComponent<ComponentAttachModel>();
        if(!attach_model)
        {
            return;
        }
        auto attach_owner = attach_model->GetAttachObject();
        if(attach_owner.expired())
        {
            return;
        }
        if(auto parent = attach_owner.lock())
        {
            // 親が攻撃コンポーネントを持っていない場合は切り離す
            if(!parent->GetComponent<WeaponAttack>())
            {
                // 反動力を加えて吹き飛ばす
                GetOwner()->AddComponent<ReactiveForce>()->SetDirection(GetOwner()->GetTranslate() - parent->GetTranslate());
                // 関節オブジェクトを削除
                Scene::Object::Release(weapon_joint_);
                weapon_joint_ = nullptr;
                // パーツから削除
                if(auto stick_part = stick->GetComponent<AttachWeaponParts>())
                {
                    stick_part->RemovePart(GetOwnerPtr());
                }

                GetOwner()->RemoveComponent(shared_from_this());
                return;
            }
        }
    }
}

//! @brief GUI表示処理
void AttachWeapon::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("AttachWeapon"))
        {
            {
                ImGui::Text("WeaponObject: %s", weapon_object_->GetName().data());
            }
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
void AttachWeapon::Exit()
{
    Super::Exit();

    if(auto model = GetOwner()->GetComponent<ComponentModel>())
    {
        // 透明度をリセット
        auto mesh_num = MV1GetMeshNum(model->GetModel());
        for(int i = 0; i < mesh_num; i++)
        {
            MV1SetMeshOpacityRate(model->GetModel(), i, 1.0f);
        }
    }

    // 関連コンポーネントの削除
    if(GetOwner()->GetComponent<AttachWeaponRemain>())
    {
        GetOwner()->RemoveComponent<AttachWeaponRemain>();
    }
    if(GetOwner()->GetComponent<AttachAttack>())
    {
        GetOwner()->RemoveComponent<AttachAttack>();
    }
    if(GetOwner()->GetComponent<WeaponAttack>())
    {
        GetOwner()->RemoveComponent<WeaponAttack>();
    }

    Scene::Object::Release(weapon_joint_);
    weapon_joint_ = nullptr;
}

//! @brief モデルローカル座標ベースの最寄りフレーム探索
//! @param model_ptr モデルコンポーネント
//! @param model_handle モデルハンドル
//! @param hit_pos ヒット位置
//! @return 最寄りフレームインデックス
int AttachWeapon::GetNearestFrame(ComponentModelPtr model_ptr, int model_handle, float3 hit_pos)
{
    int nearest_frame  = -1;
    float min_distance = FLT_MAX;

    int frame_num = MV1GetFrameNum(model_handle);
    if(frame_num <= 0)
    {
        return -1;
    }

    // 現在使用中のフレームを取得
    std::vector<bool> occupied_frames(frame_num, false);
    for(auto& object : Scene::Object::GetArray<Object>())
    {
        auto attach = object->GetComponent<AttachWeapon>();
        if(!object->GetStatus(Object::StatusBit::Alive) || !attach || !attach->weapon_joint_ || !attach->weapon_joint_->GetStatus(Object::StatusBit::Alive) ||
           attach->weapon_object_.get() != model_ptr->GetOwner())
        {
            continue;
        }
        if(attach->weapon_frame_index_ >= 0 && attach->weapon_frame_index_ < frame_num)
        {
            occupied_frames[attach->weapon_frame_index_] = true;
        }
    }

    for(int i = 0; i < frame_num; i++)
    {
        if(occupied_frames[i])
        {
            continue;
        }
        // 名前がBoneで始まるフレームだけを対象にする
        const char* frame_name = model_ptr->GetNodesNamePChar()[i];
        if(strncmp(frame_name, "Bone", 4) != 0)
        {
            continue;
        }
        VECTOR frame_pos = MV1GetFramePosition(model_handle, i);
        float dist       = VSize(VSub(frame_pos, cast(hit_pos)));
        if(dist < min_distance)
        {
            min_distance  = dist;
            nearest_frame = i;
        }
    }
    return nearest_frame;
}

//! @brief 連結可能なフレームがあるか
//! @param position 位置
//! @param weapon_ptr 武器オブジェクト(自分)
//! @param object_ptr オブジェクト(相手)
//! @return 連結可能か
bool AttachWeapon::CanAttach(float3 position, ObjectPtr weapon_ptr, ObjectPtr object_ptr)
{
    if(!weapon_ptr || !object_ptr || weapon_ptr == object_ptr)
    {
        return false;
    }
    auto weapon_model = weapon_ptr->GetComponent<ComponentModel>();
    auto model        = object_ptr->GetComponent<ComponentModel>();
    return weapon_model && model && GetNearestFrame(weapon_model, weapon_model->GetModelHandle(), position) >= 0 &&
           GetNearestFrame(model, model->GetModelHandle(), position) >= 0;
}

//! @brief 武器を設定
//! @detail 自分を武器オブジェクトにアタッチします
//! @param position 位置
//! @param weapon_ptr 武器オブジェクト
void AttachWeapon::SetWeapon(float3 position, ObjectPtr weapon_ptr)
{
    weapon_object_    = weapon_ptr;
    auto weapon_model = weapon_ptr->GetComponent<ComponentModel>();
    auto model        = GetOwner()->GetComponent<ComponentModel>();
    if(!weapon_model || !model)
    {
        return;
    }

    if(!weapon_joint_)
    {
        // ワールド座標での最近ノード探索を使用
        weapon_frame_index_ = GetNearestFrame(weapon_model, weapon_model->GetModelHandle(), position);
        int frame           = GetNearestFrame(model, model->GetModelHandle(), position);
        if(weapon_frame_index_ < 0 || frame < 0)
        {
            return;
        }

        // 耐久値コンポーネントを追加
        if(!GetOwner()->GetComponent<AttachWeaponRemain>())
        {
            GetOwner()->AddComponent<AttachWeaponRemain>();
        }

        // 関節オブジェクトの作成
        weapon_joint_ = Scene::Object::Create<Object>()
                            ->SetTranslate(model->GetNodePosition(frame))
                            ->SetRotationAxisXYZ(GetOwner()->GetRotationAxisXYZ())
                            ->SetName("JointWeapon");

        // アタッチモデルの設定
        auto attach_model                 = weapon_joint_->AddComponent<ComponentAttachModel>();
        std::string_view attach_node_name = weapon_model->GetNodesName()[weapon_frame_index_];
        attach_model->SetAttachObject(weapon_ptr, attach_node_name);

        float3 node_world_pos = model->GetNodePosition(frame);
        float3 owner_pos      = GetOwner()->GetTranslate();

        offset_ = node_world_pos - owner_pos;

        // コリジョンサイズを少し大きくする
        if(auto col = GetOwner()->GetComponent<ComponentCollisionCapsule>())
        {
            col->SetHeight(col->GetHeight() * 1.2f);
            col->SetRadius(col->GetRadius() * 1.2f);
        }
        if(auto col_box = GetOwner()->GetComponent<ComponentCollisionBox>())
        {
            col_box->SetSize(col_box->GetSize() * 1.2f);
        }
    }
}

//! @brief ヒット位置を設定
//! @param hit_pos ヒット位置
void AttachWeapon::SetHitPosition(float3 hit_pos)
{
    weapon_hit_pos_ = hit_pos;
}

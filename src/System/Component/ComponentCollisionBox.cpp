//---------------------------------------------------------------------------
//! @file   ComponentCollisionBox.cpp
//! @brief  Boxコリジョンコンポーネント
//---------------------------------------------------------------------------
#include <System/Component/ComponentCollisionBox.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionLine.h>
#include <System/Component/ComponentTransform.h>
#include <System/Component/ComponentModel.h>
#include <System/Object.h>
#include <System/Scene.h>

void ComponentCollisionBox::Init()
{
    __super::Init();
}

void ComponentCollisionBox::Update()
{
    __super::Update();
}

// ここで当たり処理後の処理を行います
void ComponentCollisionBox::PostUpdate()
{
    __super::PostUpdate();
    old_transform_ = GetWorldMatrix();
}

void ComponentCollisionBox::Draw()
{
    __super::Draw();

    if((!IsShowDebug() || !IsShowGrid()) && !collision_status_.is(CollisionBit::ShowInGame))
    {
        return;
    }

    auto trans = GetWorldMatrix();

    float scale_x = length(trans.axisVectorX());
    float scale_y = length(trans.axisVectorY());
    float scale_z = length(trans.axisVectorZ());

    float width  = width_;
    float height = height_;
    float depth  = depth_;

    if(attach_node_ < 0)
    {
        // モデルアタッチしてない場合のみサイズ変更
        width  *= scale_x;
        height *= scale_y;
        depth  *= scale_z;
    }

    float3 pos = trans.translate();

    // ボックスの8頂点を計算
    float3 halfSize = {width * 0.5f, height * 0.5f, depth * 0.5f};

    float3 vertices[8];
    vertices[0] = pos + trans.axisX() * -halfSize.x + trans.axisY() * -halfSize.y + trans.axisZ() * -halfSize.z;
    vertices[1] = pos + trans.axisX() * halfSize.x + trans.axisY() * -halfSize.y + trans.axisZ() * -halfSize.z;
    vertices[2] = pos + trans.axisX() * halfSize.x + trans.axisY() * halfSize.y + trans.axisZ() * -halfSize.z;
    vertices[3] = pos + trans.axisX() * -halfSize.x + trans.axisY() * halfSize.y + trans.axisZ() * -halfSize.z;
    vertices[4] = pos + trans.axisX() * -halfSize.x + trans.axisY() * -halfSize.y + trans.axisZ() * halfSize.z;
    vertices[5] = pos + trans.axisX() * halfSize.x + trans.axisY() * -halfSize.y + trans.axisZ() * halfSize.z;
    vertices[6] = pos + trans.axisX() * halfSize.x + trans.axisY() * halfSize.y + trans.axisZ() * halfSize.z;
    vertices[7] = pos + trans.axisX() * -halfSize.x + trans.axisY() * halfSize.y + trans.axisZ() * halfSize.z;

    SetUseLighting(FALSE);
    SetLightEnable(FALSE);

    u32 color = GetColor(0, 255, 0);

    // 下面
    DrawLine3D(cast(vertices[0]), cast(vertices[1]), color);
    DrawLine3D(cast(vertices[1]), cast(vertices[2]), color);
    DrawLine3D(cast(vertices[2]), cast(vertices[3]), color);
    DrawLine3D(cast(vertices[3]), cast(vertices[0]), color);

    // 上面
    DrawLine3D(cast(vertices[4]), cast(vertices[5]), color);
    DrawLine3D(cast(vertices[5]), cast(vertices[6]), color);
    DrawLine3D(cast(vertices[6]), cast(vertices[7]), color);
    DrawLine3D(cast(vertices[7]), cast(vertices[4]), color);

    // 縦線
    DrawLine3D(cast(vertices[0]), cast(vertices[4]), color);
    DrawLine3D(cast(vertices[1]), cast(vertices[5]), color);
    DrawLine3D(cast(vertices[2]), cast(vertices[6]), color);
    DrawLine3D(cast(vertices[3]), cast(vertices[7]), color);

    SetLightEnable(TRUE);
    SetUseLighting(TRUE);
}

void ComponentCollisionBox::Exit()
{
    __super::Exit();
}

//! @brief GUI処理
void ComponentCollisionBox::GUI()
{
    // オーナーの取得
    assert(GetOwner());
    auto obj_name = GetOwner()->GetName();

    ImGui::Begin(obj_name.data());
    {
        ImGui::Separator();
        auto ui_name = std::string("Collision Box:") + std::to_string(collision_id_);
        if(ImGui::TreeNode(ui_name.c_str()))
        {
            if(ImGui::Button(u8"削除"))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }

            // コリジョン情報を出す
            GUICollisionData();

            std::string colname = u8"COL:" + std::to_string(collision_id_) + "/ ";

            float* mat = GetColMatrixFloat();
            float matrixTranslation[3], matrixRotation[3], matrixScale[3];
            DecomposeMatrixToComponents(mat, matrixTranslation, matrixRotation, matrixScale);
            ImGui::DragFloat3((colname + u8"座標(T)").c_str(), matrixTranslation, 0.01f);
            ImGui::DragFloat3((colname + u8"回転(R)").c_str(), matrixRotation, 0.1f);
            RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, mat);

            ImGui::DragFloat((colname + u8"幅(W)").c_str(), &width_, 0.01f, 0.01f, 1000.0f, "%.2f");
            ImGui::DragFloat((colname + u8"高さ(H)").c_str(), &height_, 0.01f, 0.01f, 1000.0f, "%.2f");
            ImGui::DragFloat((colname + u8"奥行き(D)").c_str(), &depth_, 0.01f, 0.01f, 1000.0f, "%.2f");

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//! @brief サイズの設定
//! @param width 幅
//! @param height 高さ
//! @param depth 奥行き
ComponentCollisionBoxPtr ComponentCollisionBox::SetSize(float width, float height, float depth)
{
    width_  = width;
    height_ = height;
    depth_  = depth;
    return std::dynamic_pointer_cast<ComponentCollisionBox>(shared_from_this());
}

//! @brief サイズの設定
//! @param size サイズ
ComponentCollisionBoxPtr ComponentCollisionBox::SetSize(const float3& size)
{
    width_  = size.x;
    height_ = size.y;
    depth_  = size.z;
    return std::dynamic_pointer_cast<ComponentCollisionBox>(shared_from_this());
}

//! @brief サイズの取得
//! @return サイズ
float3 ComponentCollisionBox::GetSize() const
{
    return {width_, height_, depth_};
}

//! @brief 当たっているかを調べる
//! @param col 相手のコリジョン
//! @return HitInfoを返す
ComponentCollisionBox::HitInfo ComponentCollisionBox::IsHit(ComponentCollisionPtr col)
{
    HitInfo info;

    switch(col->GetCollisionType())
    {
    case ComponentCollision::CollisionType::LINE:
        // @todo Box vs Line
        break;
    case ComponentCollision::CollisionType::TRIANGLE:
        // @todo Box vs Triangle
        break;
    case ComponentCollision::CollisionType::SPHERE:
        // @todo Box vs Sphere
        break;
    case ComponentCollision::CollisionType::CAPSULE:
        return isHit(std::dynamic_pointer_cast<ComponentCollisionBox>(shared_from_this()), std::dynamic_pointer_cast<ComponentCollisionCapsule>(col));
        break;
    case ComponentCollision::CollisionType::MODEL:
        return isHit(std::dynamic_pointer_cast<ComponentCollisionBox>(shared_from_this()), std::dynamic_pointer_cast<ComponentCollisionModel>(col));
        break;
    case ComponentCollision::CollisionType::BOX:
        return isHit(std::dynamic_pointer_cast<ComponentCollisionBox>(shared_from_this()), std::dynamic_pointer_cast<ComponentCollisionBox>(col));
        break;
    }

    return info;
}

//! @brief ワールドMatrixの取得
//! @return 他のコンポーネントも含めた位置
const matrix ComponentCollisionBox::GetWorldMatrix() const
{
    auto transform = collision_transform_;

    if(IsCollisionStatus(ComponentCollision::CollisionBit::DisableTransform))
    {
        return transform;
    }

    auto obj = GetOwner();
    if(attach_node_ >= 0)
    {
        auto mdl = obj->GetComponent<ComponentModel>();
        if(mdl)
        {
            transform = mul(transform, attach_node_matrix_);
        }
    }
    else
    {
        auto cmp = obj->GetComponent<ComponentTransform>();
        if(cmp)
        {
            transform = mul(transform, cmp->GetWorldMatrix());
        }
    }

    return transform;
}

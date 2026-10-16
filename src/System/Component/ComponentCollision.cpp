//---------------------------------------------------------------------------
//! @file   ComponentCollision.h
//! @brief  コリジョンコンポーネント(ベースクラス)
//---------------------------------------------------------------------------
#include <System/Component/ComponentCollision.h>
#include <System/Component/ComponentTransform.h>

#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCollisionLine.h>
#include <System/Component/ComponentCollisionBox.h>
#include <System/Component/ComponentModel.h>

#include <System/Utils/HelperLib.h>

#include <System/Object.h>

#include <algorithm>

namespace
{
// 大きな値を取得する
float3 merge(float3 v1, float3 v2)
{
    float3 vh{v2.x, v2.y, v2.z};

    if(fabs(v1.x) > fabs(v2.x))
    {
        vh.x = v1.x;
    }

    if(fabs(v1.y) > fabs(v2.y))
    {
        vh.y = v1.y;
    }

    if(fabs(v1.z) > fabs(v2.z))
    {
        vh.z = v1.z;
    }

    return vh;
}
}   // namespace

ComponentCollision::ComponentCollision()
{
    // 複数設定可能とする
    SetStatus(Component::StatusBit::SameType, true);
}

void ComponentCollision::Construct(ObjectPtr owner)
{
    // 識別子設定
    assert(owner);
    owner_    = owner;
    auto cmps = owner->GetComponents<ComponentCollision>();
    int max   = -1;
    for(ComponentCollisionPtr cmp : cmps)
    {
        if((int)cmp->collision_id_ > max)
        {
            max = (int)cmp->collision_id_;
        }
    }

    // 割り出したIDを collision_id_ に入れる
    collision_id_ = (u32)(max + 1);
}

//! @brief 当たった情報はコールバックで送られてくる
//! @param hitInfo 当たった情報
//! @details 当たった回数分ここに来ます
void ComponentCollision::OnHit(const HitInfo& hitInfo)
{
    auto obj = GetOwner();

    constexpr float PUSH_THRESHOLD = 0.01f;
    if(length(hitInfo.push_) > (float1)PUSH_THRESHOLD)
    {
        float3 current_pos = obj->GetTranslate();
        obj->SetTranslate(current_pos + hitInfo.push_);
    }
    if(hitInfo.push_.y > 0.0f)
    {
        // 重力を完全にリセット
        now_gravity_  = {0.0f, 0.0f, 0.0f};
        calc_gravity_ = {0.0f, 0.0f, 0.0f};
        GetOwner()->SetGravity({0.0f, 0.0f, 0.0f});
    }

    // Staticな物質にぶつかった場合、gravity_を下げる
    if(hitInfo.hit_collision_->GetMass() < 0)
    {
        auto vec   = obj->GetTranslate() - obj->GetOldWorldMatrix().translate();
        float3 nvc = {0.0f, -1.0f, 0.0f};
        if(length(vec).x <= 0 || length(now_gravity_).x <= 0)
        {
            now_gravity_  = 0.0f;
            calc_gravity_ = 0.0f;
            GetOwner()->SetGravity(calc_gravity_);
        }
        else
        {
            nvc           = normalize(vec);
            float d       = dot(normalize(now_gravity_), nvc);
            now_gravity_ *= ((1 - (d * d)) * 0.1f);
            //GetOwner()->SetGravity( calc_gravity_ );
        }
    }

    obj->OnHit(hitInfo);
}

#if 0
void ComponentCollision::SetName(std::string_view name)
{
	name_ = name;
}

std::string_view ComponentCollision::GetName()
{
	return name_;
}
#endif

const char* const collisionGroupName[] = {
    "WALL", "GROUND", "PLAYER", "ENEMY ", "WEAPON", "ITEM  ", "CAMERA", "ETC", "ENEMY_WEAPON", "ENEMY_WEAPON2", "DODGE", "STAGE_TRIGGER", "ULTIMATE_ATTACK"};

void ComponentCollision::GUICollisionData(bool use_attach)
{
    // コリジョンデータ表示
    guiCollisionData();

    // アタッチが存在必要な時
    if(use_attach)
    {
        guiCollisionDataAttach();
    }
}

void ComponentCollision::LateUpdate()
{
    __super::LateUpdate();

    // モデルにアタッチしている場合
    // attach_node_matrix_ にモデルのNode位置を設定する
    if(attach_node_ >= 0)
    {
        attach_node_matrix_ = matrix::identity();
        if(auto mdl = GetOwner()->GetComponent<ComponentModel>())
        {
            attach_node_matrix_ = MV1GetFrameLocalWorldMatrix(mdl->GetModel(), attach_node_);
        }
    }
#ifdef USE_JOLT_PHYSICS
#else
    // 重力加速度
    if(use_gravity_)
    {
        float old_flg       = now_gravity_.y > 0 ? 1.0f : -1.0f;
        float3 old_gravity  = ((now_gravity_ * now_gravity_ * k_gravity_ * k_gravity_) * old_flg) / 2;
        now_gravity_       += gravity_ * GetDeltaTime();

        float flg     = now_gravity_.y > 0 ? 1.0f : -1.0f;
        calc_gravity_ = ((now_gravity_ * now_gravity_ * k_gravity_ * k_gravity_) * flg) / 2 - old_gravity;
        GetOwner()->SetGravity(calc_gravity_);
    }
#endif
}

void ComponentCollision::PostUpdate()
{
    //! 古いマトリクスを更新します
    old_transform_ = collision_transform_;
    if(attach_node_ >= 0)
    {
        attach_node_old_matrix_ = attach_node_matrix_;
    }
}

//! @brief GUI処理
void ComponentCollision::GUI() {}

void ComponentCollision::AttachToModel(int node)
{
    attach_node_ = node;
#ifdef USE_JOLT_PHYSICS
    if(GetRigidBody())
    {
        GetRigidBody()->setGravityFactor(0.0f);
    }
#endif USE_JOLT_PHYSICS
}

void ComponentCollision::AttachToModel(const std::string_view name)
{
    if(auto mdl = GetOwner()->GetComponent<ComponentModel>())
    {
        attach_node_ = mdl->GetNodeIndex(name);
#ifdef USE_JOLT_PHYSICS
        if(GetRigidBody())
        {
            GetRigidBody()->setGravityFactor(0.0f);
        }
#endif USE_JOLT_PHYSICS
    }
}

void ComponentCollision::guiCollisionData()
{
    //----------------------------------------------------------------------------
    // GUIコリジョン基本情報
    //----------------------------------------------------------------------------
    auto str = u8"コリジョンタイプ : " + CollisionTypeName[(u32)collision_type_];
    ImGui::Text(str.c_str());
    ImGui::Separator();
    ImGui::Text(u8"コリジョングループ: ");
    ImGui::SameLine();
    int index = GetCollisionGroupIndex();
    if(ImGui::Combo(u8"##コリジョングループ", &index, collisionGroupName, (sizeof(collisionGroupName) / sizeof(collisionGroupName[0]))))
    {
        collision_group_ = static_cast<CollisionGroup>(1 << index);
    }
    ImGui::Separator();

    //----------------------------------------------------------------------------
    // ゲーム中のヒット表示
    //----------------------------------------------------------------------------
    //  ImGui::CheckboxFlags(u8"初期化済み", &collision_status_.get(), 1 << (u32)CollisionBit::Initialized);
    ImGui::CheckboxFlags(u8"ヒットしない", &collision_status_.get(), 1 << (u32)CollisionBit::DisableHit);
    ImGui::CheckboxFlags(u8"ゲーム中表示", &collision_status_.get(), 1 << (u32)CollisionBit::ShowInGame);
    ImGui::Separator();

    //----------------------------------------------------------------------------
    // Hitするグループの設定
    //----------------------------------------------------------------------------
    if(ImGui::TreeNode(u8"Hitするグループ"))
    {
        ImGui::CheckboxFlags("WALL", (u32*)&collision_hit_, (u32)CollisionGroup::WALL);
        ImGui::CheckboxFlags("GROUND", (u32*)&collision_hit_, (u32)CollisionGroup::GROUND);
        ImGui::CheckboxFlags("PLAYER", (u32*)&collision_hit_, (u32)CollisionGroup::PLAYER);
        ImGui::CheckboxFlags("ENEMY", (u32*)&collision_hit_, (u32)CollisionGroup::ENEMY);
        ImGui::CheckboxFlags("WEAPON", (u32*)&collision_hit_, (u32)CollisionGroup::WEAPON);
        ImGui::CheckboxFlags("ITEM", (u32*)&collision_hit_, (u32)CollisionGroup::ITEM);
        ImGui::CheckboxFlags("CAMERA", (u32*)&collision_hit_, (u32)CollisionGroup::CAMERA);
        ImGui::CheckboxFlags("ETC", (u32*)&collision_hit_, (u32)CollisionGroup::ETC);
        ImGui::CheckboxFlags("ENEMY_WEAPON", (u32*)&collision_hit_, (u32)CollisionGroup::ENEMY_WEAPON);
        ImGui::CheckboxFlags("ENEMY_WEAPON2", (u32*)&collision_hit_, (u32)CollisionGroup::ENEMY_WEAPON2);
        ImGui::CheckboxFlags("DODGE", (u32*)&collision_hit_, (u32)CollisionGroup::DODGE);
        ImGui::CheckboxFlags("STAGE_TRIGGER", (u32*)&collision_hit_, (u32)CollisionGroup::STAGE_TRIGGER);
        ImGui::CheckboxFlags("ULTIMATE_ATTACK", (u32*)&collision_hit_, (u32)CollisionGroup::ULTIMATE_ATTACK);
        ImGui::TreePop();
    }

    //----------------------------------------------------------------------------
    // Hitするがオーバーラップするグループ
    //----------------------------------------------------------------------------
    if(ImGui::TreeNode(u8"オーバーラップするグループ"))
    {
        ImGui::CheckboxFlags("WALL", (u32*)&collision_overlap_, (u32)CollisionGroup::WALL);
        ImGui::CheckboxFlags("GROUND", (u32*)&collision_overlap_, (u32)CollisionGroup::GROUND);
        ImGui::CheckboxFlags("PLAYER", (u32*)&collision_overlap_, (u32)CollisionGroup::PLAYER);
        ImGui::CheckboxFlags("ENEMY", (u32*)&collision_overlap_, (u32)CollisionGroup::ENEMY);
        ImGui::CheckboxFlags("WEAPON", (u32*)&collision_overlap_, (u32)CollisionGroup::WEAPON);
        ImGui::CheckboxFlags("ITEM", (u32*)&collision_overlap_, (u32)CollisionGroup::ITEM);
        ImGui::CheckboxFlags("CAMERA", (u32*)&collision_overlap_, (u32)CollisionGroup::CAMERA);
        ImGui::CheckboxFlags("ETC", (u32*)&collision_overlap_, (u32)CollisionGroup::ETC);
        ImGui::CheckboxFlags("ENEMY_WEAPON", (u32*)&collision_hit_, (u32)CollisionGroup::ENEMY_WEAPON);
        ImGui::CheckboxFlags("ENEMY_WEAPON2", (u32*)&collision_hit_, (u32)CollisionGroup::ENEMY_WEAPON2);
        ImGui::CheckboxFlags("DODGE", (u32*)&collision_hit_, (u32)CollisionGroup::DODGE);
        ImGui::CheckboxFlags("STAGE_TRIGGER", (u32*)&collision_hit_, (u32)CollisionGroup::STAGE_TRIGGER);
        ImGui::CheckboxFlags("ULTIMATE_ATTACK", (u32*)&collision_hit_, (u32)CollisionGroup::ULTIMATE_ATTACK);
        ImGui::TreePop();
    }
    ImGui::Separator();

    //----------------------------------------------------------------------------
    // 重力設定
    //----------------------------------------------------------------------------
    ImGui::Checkbox(u8"重力を使用する", &use_gravity_);
    ImGui::DragFloat3(u8"重力加速度", (float*)&gravity_);

    if(ImGui::DragFloat(u8"質量", (float*)&collision_mass_, 0.001f, 0, 1000000.0f))
    {
        collision_mass_ = std::min(1000000.0f, std::max(collision_mass_, 0.001f));
    }
}

void ComponentCollision::guiCollisionDataAttach()
{
    // モデルにアタッチしているかを調べる
    if(auto cmp = GetOwner()->GetComponent<ComponentModel>())
    {
        // GUIでの AttachNodeの切り替えに対応させる
        bool attach = false;
        if(attach_node_ >= 0)
        {
            // 既にアタッチ済み
            attach = true;
            if(ImGui::Checkbox("AttachNode", &attach))
            {
                if(!attach)
                {
                    attach_node_ = -1;
                }
            }
        }
        else
        {
            // アタッチしていない
            if(ImGui::Checkbox("AttachNode", &attach))
            {
                if(attach)
                {
                    attach_node_ = 0;
                }
            }
        }

        // GUIノードを列挙します
        auto items = cmp->GetNodesNamePChar();
        if(ImGui::Combo("Node", &attach_node_, items.data(), (int)items.size()))
        {
            // 切り替えたとき
            collision_transform_ = matrix::identity();
        }
    }
}

//! @brief Capsule VS Sphere
//! @param col1 Capsuleコリジョン
//! @param col2 Sphere コリジョン
//! @return 当たり情報
ComponentCollision::HitInfo ComponentCollision::isHit(ComponentCollisionCapsulePtr col1, ComponentCollisionSpherePtr col2)
{
    ComponentCollision::HitInfo info{};

    // 自分のコリジョン
    float3 cpos1 = col1->GetTranslate();
    float3 cpos2 = col1->GetVectorAxisY() * col1->GetHeight() + cpos1;
    float cs     = 1.0f;   //< スケール

    float3 epos1;
    float es = 1.0f;

    // モデルアタッチ
    if(col1->attach_node_ >= 0)
    {
        if(auto mdl = col1->GetOwner()->GetComponent<ComponentModel>())
        {
            cpos1 = mul(float4(cpos1, 1), col1->attach_node_matrix_).xyz;
            cpos2 = mul(float4(cpos2, 1), col1->attach_node_matrix_).xyz;
            cpos2 = normalize(cpos2 - cpos1) * col1->GetHeight() + cpos1;
        }
    }
    else
    {
        // ComponentTransform(オブジェクト姿勢)
        if(auto cmp = col1->GetOwner()->GetComponent<ComponentTransform>())
        {
            // 高さに回転とスケールを掛け合わせる
            cpos1 = mul(float4(cpos1, 1), cmp->GetMatrix()).xyz;
            cpos2 = mul(float4(cpos2, 1), cmp->GetMatrix()).xyz;
            // 半径はXZで平均としておく
            cs = (length(cmp->GetMatrix().axisX()) + length(cmp->GetMatrix().axisZ())) / 2;
        }
    }

    // モデルアタッチ
    if(col2->attach_node_ >= 0)
    {
        if(auto mdl = col1->GetOwner()->GetComponent<ComponentModel>())
        {
            epos1 = col2->GetTranslate();
            epos1 = mul(float4(epos1, 1), col2->attach_node_matrix_).xyz;
        }
    }
    else
    {
        // ComponentTransform(オブジェクト姿勢)
        if(auto cmp = col2->GetOwner()->GetComponent<ComponentTransform>())
        {
            epos1 = mul(col2->GetMatrix(), cmp->GetMatrix())._41_42_43;
            // pos1 = mul( float4( pos1, 0 ) , cmp->GetMatrix() ).xyz;
            // pos1 += cmp->GetTranslate().xyz;
            float sx = length(cmp->GetVectorAxisX());
            float sy = length(cmp->GetVectorAxisY());
            float sz = length(cmp->GetVectorAxisZ());
            es       = (sx + sy + sz) / 3.0f;
        }
    }

    float cr  = col1->GetRadius() * cs;
    float3 cv = normalize(cpos1 - cpos2);
    VECTOR c1 = cast(cpos1 - cv * cr);
    VECTOR c2 = cast(cpos2 + cv * cr);

    float er  = col2->GetRadius() * es;
    VECTOR e1 = cast(epos1);

    // 跳ね返り点が欲しいため、HitCheck_Capsule_Capsuleは使わない
    SEGMENT_POINT_RESULT result;
    Segment_Point_Analyse(&c1, &c2, &e1, &result);

    if(result.Seg_Point_MinDist_Square < (cr + er) * (cr + er))
    {
        // 線と線で一番近くなる点を求め、ベクトル化する
        // 最も近い点
        float3 c0  = cast(result.Seg_MinDist_Pos);
        float3 e0  = cast(e1);
        float3 vec = e0 - c0;   // 調べたほうの跳ね返りの方向(100%)
        float len  = length(vec);
        if(abs(len) <= abs(len) * FLT_EPSILON)
        {
            // 全く同じ位置にいる場合はz移動する形にしておく
            vec = {0, 0, 1};
        }

        float3 vs  = normalize(vec) * (cr + er);
        vec       -= vs;

        // このpush_は、調べたほうの押し戻し方向100%で作成する
        info.push_         = vec;
        info.hit_          = true;
        info.hit_position_ = (e0 + c0) * 0.5f;
    }

    return info;
}

//! @brief Sphere VS Capsule
//! @param col1 Sphereコリジョン
//! @param col2 Capsule コリジョン
//! @return 当たり情報
ComponentCollision::HitInfo ComponentCollision::isHit(ComponentCollisionSpherePtr col1, ComponentCollisionCapsulePtr col2)
{
    auto hit  = isHit(col2, col1);
    hit.push_ = -hit.push_;   // push方向を反対にする
    return hit;
}

//! @brief Capsule VS Capsule
//! @param col1 Capsuleコリジョン
//! @param col2 Capsule コリジョン
//! @return 当たり情報
ComponentCollision::HitInfo ComponentCollision::isHit(ComponentCollisionCapsulePtr col1, ComponentCollisionCapsulePtr col2)
{
    ComponentCollision::HitInfo info{};

    // 自分のコリジョン
    float3 cpos1 = col1->GetTranslate();
    float3 cpos2 = normalize(col1->GetVectorAxisY()) * col1->GetHeight() + cpos1;

    //前回の座標情報
    float3 cpos1_old = col1->GetOldWorldMatrix().translate();
    float3 cpos2_old = normalize(col1->GetOldWorldMatrix().axisY()) * col1->GetHeight() + cpos1_old;

    float cs     = 1.0f;   //< スケール
    float cs_old = 1.0f;   //< スケール

    // モデルアタッチ
    if(col1->attach_node_ >= 0)
    {
        if(auto mdl = col1->GetOwner()->GetComponent<ComponentModel>())
        {
            cpos1 = mul(float4(cpos1, 1), col1->attach_node_matrix_).xyz;
            cpos2 = mul(float4(cpos2, 1), col1->attach_node_matrix_).xyz;
            //cpos2 = normalize(cpos2 - cpos1) * col1->GetHeight() + cpos1;
            cpos1_old = mul(float4(cpos1_old, 1), col1->attach_node_old_matrix_).xyz;
            cpos2_old = mul(float4(cpos2_old, 1), col1->attach_node_old_matrix_).xyz;

            cs     = (length(col1->attach_node_matrix_.axisX()) + length(col1->attach_node_matrix_.axisZ())) / 2.0f;
            cs_old = (length(col1->attach_node_old_matrix_.axisX()) + length(col1->attach_node_old_matrix_.axisZ())) / 2.0f;
        }
    }
    else
    {
        // ComponentTransform(オブジェクト姿勢)
        if(auto cmp = col1->GetOwner()->GetComponent<ComponentTransform>())
        {
            auto& mtx = cmp->GetWorldMatrix();
            // 高さに回転とスケールを掛け合わせる
            cpos1        = mul(float4(cpos1, 1), mtx).xyz;
            cpos2        = mul(float4(cpos2, 1), mtx).xyz;
            auto old_mtx = cmp->GetOldWorldMatrix();
            cpos1_old    = mul(float4(cpos1_old, 1), old_mtx).xyz;
            cpos2_old    = mul(float4(cpos2_old, 1), old_mtx).xyz;

            // 半径はXZで平均としておく
            cs     = (length(mtx.axisX()) + length(mtx.axisZ())) / 2;
            cs_old = (length(old_mtx.axisX()) + length(old_mtx.axisZ())) / 2;
        }
    }

    // 相手のコリジョン
    float3 epos1 = col2->GetTranslate();
    float3 epos2 = normalize(col2->GetVectorAxisY()) * col2->GetHeight() + epos1;

    //前回の座標情報
    float3 epos1_old = col2->GetOldWorldMatrix().translate();
    float3 epos2_old = normalize(col2->GetOldWorldMatrix().axisY()) * col2->GetHeight() + epos1_old;
    float es         = 1.0f;   //< スケール
    float es_old     = 1.0f;   //< スケール

    // モデルアタッチ
    if(col2->attach_node_ >= 0)
    {
        if(auto mdl = col2->GetOwner()->GetComponent<ComponentModel>())
        {
            epos1 = mul(float4(epos1, 1), col2->attach_node_matrix_).xyz;
            epos2 = mul(float4(epos2, 1), col2->attach_node_matrix_).xyz;
            //epos2 = normalize(epos2 - epos1) * col2->GetHeight() + epos1;
            epos1_old = mul(float4(epos1_old, 1), col2->attach_node_old_matrix_).xyz;
            epos2_old = mul(float4(epos2_old, 1), col2->attach_node_old_matrix_).xyz;

            es     = (length(col2->attach_node_matrix_.axisX()) + length(col2->attach_node_matrix_.axisZ())) / 2.0f;
            es_old = (length(col2->attach_node_old_matrix_.axisX()) + length(col2->attach_node_old_matrix_.axisZ())) / 2.0f;
        }
    }
    else
    {
        // ComponentTransform(オブジェクト姿勢)
        if(auto cmp = col2->GetOwner()->GetComponent<ComponentTransform>())
        {
            auto& mtx = cmp->GetWorldMatrix();
            // 高さに回転とスケールを掛け合わせる
            epos1        = mul(float4(epos1, 1), mtx).xyz;
            epos2        = mul(float4(epos2, 1), mtx).xyz;
            auto old_mtx = cmp->GetOldWorldMatrix();
            epos1_old    = mul(float4(epos1_old, 1), old_mtx).xyz;
            epos2_old    = mul(float4(epos2_old, 1), old_mtx).xyz;
            // 半径はXZで平均としておく
            es     = (length(mtx.axisX()) + length(mtx.axisZ())) / 2;
            es_old = (length(old_mtx.axisX()) + length(old_mtx.axisZ())) / 2;
        }
    }

    static constexpr int SUBSTEPS        = 3;
    static constexpr float SUBSTEP_DELTA = 1.0f / SUBSTEPS;
    for(int i = 0; i < SUBSTEPS; i++)
    {
        float3 work_cpos1 = lerp(cpos1_old, cpos1, i * SUBSTEP_DELTA);
        float3 work_cpos2 = lerp(cpos2_old, cpos2, i * SUBSTEP_DELTA);
        float cr          = col1->GetRadius() * std::lerp(cs_old, cs, i * SUBSTEP_DELTA);
        float3 cv         = normalize(work_cpos1 - work_cpos2);
        VECTOR c1         = cast(work_cpos1 - (cv * cr));
        VECTOR c2         = cast(work_cpos2 + (cv * cr));

        float3 work_epos1 = lerp(epos1_old, epos1, i * SUBSTEP_DELTA);
        float3 work_epos2 = lerp(epos2_old, epos2, i * SUBSTEP_DELTA);
        float er          = col2->GetRadius() * std::lerp(es_old, es, i * SUBSTEP_DELTA);
        float3 ev         = normalize(work_epos1 - work_epos2);
        VECTOR e1         = cast(work_epos1 - (ev * er));
        VECTOR e2         = cast(work_epos2 + (ev * er));

        // 跳ね返り点が欲しいため、HitCheck_Capsule_Capsuleは使わない
        SEGMENT_SEGMENT_RESULT result;
        Segment_Segment_Analyse(&c1, &c2, &e1, &e2, &result);

#if 0   // 当たり不具合チェック用
        if(col1->attach_node_ > 0)
        {
            DrawCapsule3D(c1, c2, cr, 10, GetColor(0, 0, 255), GetColor(0, 0, 255), FALSE);
        }

        if(col2->attach_node_ > 0)
        {
            DrawCapsule3D(e1, e2, er, 10, GetColor(0, 0, 255), GetColor(0, 0, 255), FALSE);
        }
#endif

        //当たっているか
        if(result.SegA_SegB_MinDist_Square < (cr + er) * (cr + er))
        {
            // 線と線で一番近くなる点を求め、ベクトル化する
            // 最も近い点
            float3 c0  = cast(result.SegA_MinDist_Pos);
            float3 e0  = cast(result.SegB_MinDist_Pos);
            float3 vec = e0 - c0;   // 調べたほうの跳ね返りの方向(100%)

            float len = length(vec);
            if(abs(len) <= abs(len) * FLT_EPSILON)
            {
                // 全く同じ位置にいる場合はz移動する形にしておく
                vec = {0, 0, 1};
            }
            float3 vs  = normalize(vec) * (cr + er);
            vec       -= vs;

            // このpush_は、調べたほうの押し戻し方向100%で作成する
            info.push_         = vec;
            info.hit_          = true;
            info.hit_position_ = (e0 + c0) * 0.5f;
            break;
        }
    }

    return info;
}

//! @brief Sphere VS Sphere
//! @param col1 Sphereコリジョン
//! @param col2 Sphere コリジョン
//! @return 当たり情報
ComponentCollision::HitInfo ComponentCollision::isHit(ComponentCollisionSpherePtr col1, ComponentCollisionSpherePtr col2)
{
    ComponentCollision::HitInfo info{};

    float3 pos1;
    float scale1 = 1.0f;

    // モデルアタッチ
    if(col1->attach_node_ >= 0)
    {
        if(auto mdl = col1->GetOwner()->GetComponent<ComponentModel>())
        {
            pos1 = col1->GetTranslate();
            pos1 = mul(float4(pos1, 1), col1->attach_node_matrix_).xyz;
        }
    }
    else
    {
        if(auto cmp = col1->GetOwner()->GetComponent<ComponentTransform>())
        {
            pos1 = mul(col1->GetMatrix(), cmp->GetWorldMatrix())._41_42_43;
            // pos1 = mul( float4( pos1, 0 ) , cmp->GetMatrix() ).xyz;
            // pos1 += cmp->GetTranslate().xyz;
            float sx = length(cmp->GetVectorAxisX());
            float sy = length(cmp->GetVectorAxisY());
            float sz = length(cmp->GetVectorAxisZ());
            scale1   = (sx + sy + sz) / 3.0f;
        }
    }

    float3 pos2;
    float scale2 = 1.0f;

    // モデルアタッチ
    if(col2->attach_node_ >= 0)
    {
        if(auto mdl = col2->GetOwner()->GetComponent<ComponentModel>())
        {
            pos2 = col2->GetTranslate();
            pos2 = mul(float4(pos2, 1), col2->attach_node_matrix_).xyz;
        }
    }
    else
    {
        if(auto cmp = col2->GetOwner()->GetComponent<ComponentTransform>())
        {
            pos2 = mul(col2->GetMatrix(), cmp->GetWorldMatrix())._41_42_43;
            // pos2 = mul( float4( pos2, 0 ), cmp->GetMatrix() ).xyz;
            // pos2 += cmp->GetTranslate().xyz;
            float sx = length(cmp->GetVectorAxisX());
            float sy = length(cmp->GetVectorAxisY());
            float sz = length(cmp->GetVectorAxisZ());
            scale2   = (sx + sy + sz) / 3.0f;
        }
    }

    if(HitCheck_Sphere_Sphere(cast(pos1), col1->GetRadius() * scale1, cast(pos2), col2->GetRadius() * scale2))
    {
        // 中間地点を当たった場所にする
        info.hit_          = true;
        info.hit_position_ = (pos1 - pos2) * 0.5f + pos2;

        // 押し出し方向
        float3 distance = pos1 - pos2;
        float len       = length(distance);
        if(abs(len) <= abs(len) * FLT_EPSILON)
        {
            // 全く同じ位置にいる場合はz移動する形にしておく
            distance = {0, 0, 1};
        }

        float3 vec    = normalize(distance);
        float reallen = col1->GetRadius() * scale1 + col2->GetRadius() * scale2;
        vec           = vec * (reallen - len);

        // このpush_は、調べたほうの押し戻し方向100%で作成する
        info.push_ = vec;
    }

    return info;
}

//! @brief Model VS Sphere
//! @param col1 Model コリジョン
//! @param col2 Sphereコリジョン
//! @return 当たり情報
ComponentCollision::HitInfo ComponentCollision::isHit(ComponentCollisionSpherePtr col1, ComponentCollisionModelPtr col2)
{
    ComponentCollision::HitInfo info{};

    // モデルが存在していない
    auto mdl = col2->GetOwner()->GetComponent<ComponentModel>();
    if(mdl == nullptr)
    {
        return info;
    }

    float3 opos{};
    float3 cpos{};
    if(col1->attach_node_ >= 0)
    {
        cpos = mul(float4(col1->GetTranslate(), 1), col1->attach_node_matrix_).xyz;
        opos = cpos;
    }
    else
    {
        // オブジェクト位置に対するコリジョン(1フレーム前)
        opos = mul(float4(col1->GetTranslate(), 1), col1->GetOwner()->GetOldWorldMatrix()).xyz;

        // オブジェクト位置に対するコリジョン
        cpos = mul(float4(col1->GetTranslate(), 1), col1->GetOwner()->GetWorldMatrix()).xyz;

        // 実際の移動できる量にする
        auto move   = cpos - opos;
        float movey = move.y;
        move.y      = 0;

        if(col1->IsUseGravity())
        {
            move   = col2->checkMovement(opos, move, difficult_to_climb_);
            move.y = movey;
            cpos   = opos + move;

            // 当たりからキャラの位置を求める
            auto rot                      = col1->GetOwner()->GetWorldMatrix();
            rot._41_42_43                 = float3{0.0f, 0.0f, 0.0f};
            auto col1r                    = mul(float4(col1->GetTranslate(), 1), rot).xyz;
            col1->GetOwner()->Translate() = cpos - col1r;
        }
    }

    float scale = 1.0f;
    if(col1->attach_node_ < 0)
    {
        auto mat = col1->GetWorldMatrix();
        float sx = length(mat.axisVectorX());
        float sy = length(mat.axisVectorY());
        float sz = length(mat.axisVectorZ());
        scale    = (sx + sy + sz) / 3.0f;
    }

    float radius = col1->GetRadius() * scale;

#if 1
    // 当たりの確認(通常)
    {
        // 戻し量
        float3 vh = 0;

        MV1_COLL_RESULT_POLY_DIM hit_poly_dim{};

        float3 ocenter = opos + info.push_;
        float3 ncenter = cpos + info.push_;

        // DrawCapsule3D( cast( ocenter ), cast( ncenter ), radius, 10, GetColor( 0, 0, 255 ), GetColor( 0, 0, 255 ), FALSE );

        // スピードは一旦無視。面倒なのでカプセルをそのまま利用
        hit_poly_dim = MV1CollCheck_Capsule(mdl->GetModel(), -1, cast(ocenter), cast(ncenter), radius);
        for(int i = 0; i < hit_poly_dim.HitNum; i++)
        {
            SEGMENT_TRIANGLE_RESULT result{};

            VECTOR v1 = cast(ocenter);
            VECTOR v2 = cast(ncenter);
            DxLib::Segment_Triangle_Analyse(
                &v1, &v2, &hit_poly_dim.Dim[i].Position[0], &hit_poly_dim.Dim[i].Position[1], &hit_poly_dim.Dim[i].Position[2], &result);

            float3 line_pos = cast(result.Seg_MinDist_Pos);
            float3 tri_pos  = cast(result.Tri_MinDist_Pos);
            float3 tri_nml  = cast(hit_poly_dim.Dim[i].Normal);

            // カプセルへの戻し方向
            if(HelperLib::Math::NearlyEqual(length(line_pos - tri_pos), 0))
            {
                float3 v = (tri_pos - ncenter);
                v        = v + (tri_nml * radius);

                vh = merge(vh, v);
            }
            else
            {
                float3 vec = normalize(line_pos - tri_pos);
                // 戻し量
                float len = radius - length(line_pos - tri_pos);
                float3 v  = vec * len;
                vh        = merge(vh, v);
            }
        }
        if(hit_poly_dim.HitNum > 0)
        {
            info.push_         += vh;
            info.hit_           = true;
            info.hit_position_ += (cpos - vh);
        }
        MV1CollResultPolyDimTerminate(hit_poly_dim);
    }
#endif
    return info;
}

//! @brief Model VS Line
//! @param col1 Modelコリジョン
//! @param col2 Capsule コリジョン
//! @return 当たり情報
ComponentCollision::HitInfo ComponentCollision::isHit(ComponentCollisionModelPtr col1, ComponentCollisionLinePtr col2)
{
    auto hit  = isHit(col2, col1);
    hit.push_ = -hit.push_;   // push方向を反対にする
    return hit;
}

//! @brief Box VS Box
//! @param col1 Boxコリジョン
//! @param col2 Box コリジョン
//! @return 当たり情報
ComponentCollision::HitInfo ComponentCollision::isHit(ComponentCollisionBoxPtr col1, ComponentCollisionBoxPtr col2)
{
    ComponentCollision::HitInfo info{};

    // ボックス1の情報取得
    auto box1_mat   = col1->GetWorldMatrix();
    float3 box1_pos = box1_mat.translate();

    float scale1_x = length(box1_mat.axisVectorX());
    float scale1_y = length(box1_mat.axisVectorY());
    float scale1_z = length(box1_mat.axisVectorZ());

    float box1_width  = col1->GetWidth();
    float box1_height = col1->GetHeight();
    float box1_depth  = col1->GetDepth();

    if(col1->attach_node_ < 0)
    {
        box1_width  *= scale1_x;
        box1_height *= scale1_y;
        box1_depth  *= scale1_z;
    }

    float3 box1_half   = {box1_width * 0.5f, box1_height * 0.5f, box1_depth * 0.5f};
    float3 box1_axis_x = normalize(box1_mat.axisX());
    float3 box1_axis_y = normalize(box1_mat.axisY());
    float3 box1_axis_z = normalize(box1_mat.axisZ());

    // ボックス2の情報取得
    auto box2_mat   = col2->GetWorldMatrix();
    float3 box2_pos = box2_mat.translate();

    float scale2_x = length(box2_mat.axisVectorX());
    float scale2_y = length(box2_mat.axisVectorY());
    float scale2_z = length(box2_mat.axisVectorZ());

    float box2_width  = col2->GetWidth();
    float box2_height = col2->GetHeight();
    float box2_depth  = col2->GetDepth();

    if(col2->attach_node_ < 0)
    {
        box2_width  *= scale2_x;
        box2_height *= scale2_y;
        box2_depth  *= scale2_z;
    }

    float3 box2_half   = {box2_width * 0.5f, box2_height * 0.5f, box2_depth * 0.5f};
    float3 box2_axis_x = normalize(box2_mat.axisX());
    float3 box2_axis_y = normalize(box2_mat.axisY());
    float3 box2_axis_z = normalize(box2_mat.axisZ());

    // 中心間のベクトル
    float3 center_vec = box2_pos - box1_pos;

    // 分離軸定理(SAT)による判定
    float min_penetration = FLT_MAX;
    float3 min_axis       = {1, 0, 0};

    // テスト用のラムダ関数
    auto test_axis = [&](float3 axis) -> bool {
        float len_sq = dot(axis, axis);
        if(len_sq < 0.0001f)
        {
            return true;   // ほぼゼロベクトルの場合はスキップ
        }

        axis = normalize(axis);

        // Box1の投影半径
        float r1 = abs(dot(box1_axis_x, axis)) * box1_half.x + abs(dot(box1_axis_y, axis)) * box1_half.y + abs(dot(box1_axis_z, axis)) * box1_half.z;

        // Box2の投影半径
        float r2 = abs(dot(box2_axis_x, axis)) * box2_half.x + abs(dot(box2_axis_y, axis)) * box2_half.y + abs(dot(box2_axis_z, axis)) * box2_half.z;

        // 中心間距離の投影
        float distance = dot(center_vec, axis);   // absを外して符号を保持

        // 分離している場合
        if(abs(distance) > r1 + r2)
        {
            return false;
        }

        // めり込み量を記録
        float penetration = r1 + r2 - abs(distance);

        // より小さいめり込みを優先
        if(penetration < min_penetration)
        {
            min_penetration = penetration;
            // distanceの符号を使って正しい方向を決定
            min_axis = (distance < 0) ? -axis : axis;
        }

        return true;
    };

    // Box1の3軸でテスト
    if(!test_axis(box1_axis_x))
    {
        return info;
    }
    if(!test_axis(box1_axis_y))
    {
        return info;
    }
    if(!test_axis(box1_axis_z))
    {
        return info;
    }

    // Box2の3軸でテスト
    if(!test_axis(box2_axis_x))
    {
        return info;
    }
    if(!test_axis(box2_axis_y))
    {
        return info;
    }
    if(!test_axis(box2_axis_z))
    {
        return info;
    }

    // 外積の9軸でテスト
    if(!test_axis(cross(box1_axis_x, box2_axis_x)))
    {
        return info;
    }
    if(!test_axis(cross(box1_axis_x, box2_axis_y)))
    {
        return info;
    }
    if(!test_axis(cross(box1_axis_x, box2_axis_z)))
    {
        return info;
    }
    if(!test_axis(cross(box1_axis_y, box2_axis_x)))
    {
        return info;
    }
    if(!test_axis(cross(box1_axis_y, box2_axis_y)))
    {
        return info;
    }
    if(!test_axis(cross(box1_axis_y, box2_axis_z)))
    {
        return info;
    }
    if(!test_axis(cross(box1_axis_z, box2_axis_x)))
    {
        return info;
    }
    if(!test_axis(cross(box1_axis_z, box2_axis_y)))
    {
        return info;
    }
    if(!test_axis(cross(box1_axis_z, box2_axis_z)))
    {
        return info;
    }

    // すべての軸で分離していない = 衝突している
    info.hit_          = true;
    info.push_         = -min_axis * min_penetration;
    info.hit_position_ = box1_pos + center_vec * 0.5f;

    return info;
}

//! @brief Box VS Capsule
//! @param col1 Boxコリジョン
//! @param col2 Capsule コリジョン
//! @return 当たり情報
ComponentCollision::HitInfo ComponentCollision::isHit(ComponentCollisionBoxPtr col1, ComponentCollisionCapsulePtr col2)
{
    ComponentCollision::HitInfo info{};

    // ボックスのワールドマトリクス取得
    auto box_mat   = col1->GetWorldMatrix();
    float3 box_pos = box_mat.translate();

    // ボックスのスケール計算
    float scale_x = length(box_mat.axisVectorX());
    float scale_y = length(box_mat.axisVectorY());
    float scale_z = length(box_mat.axisVectorZ());

    // ボックスのサイズ取得(スケール適用)
    float box_width  = col1->GetWidth() * scale_x;
    float box_height = col1->GetHeight() * scale_y;
    float box_depth  = col1->GetDepth() * scale_z;
    float3 half_size = {box_width * 0.5f, box_height * 0.5f, box_depth * 0.5f};

    // カプセルの情報取得
    float3 cap_pos1 = col2->GetTranslate();
    float3 cap_pos2 = normalize(col2->GetVectorAxisY()) * col2->GetHeight() + cap_pos1;
    float cap_scale = 1.0f;

    // カプセルのワールド座標変換
    if(col2->attach_node_ >= 0)
    {
        if(auto mdl = col2->GetOwner()->GetComponent<ComponentModel>())
        {
            cap_pos1 = mul(float4(cap_pos1, 1), col2->attach_node_matrix_).xyz;
            cap_pos2 = mul(float4(cap_pos2, 1), col2->attach_node_matrix_).xyz;
            cap_pos2 = normalize(cap_pos2 - cap_pos1) * col2->GetHeight() + cap_pos1;
        }
    }
    else
    {
        if(auto cmp = col2->GetOwner()->GetComponent<ComponentTransform>())
        {
            auto& mtx = cmp->GetWorldMatrix();
            cap_pos1  = mul(float4(cap_pos1, 1), mtx).xyz;
            cap_pos2  = mul(float4(cap_pos2, 1), mtx).xyz;
            cap_scale = (length(mtx.axisX()) + length(mtx.axisZ())) / 2;
        }
    }

    float cap_radius = col2->GetRadius() * cap_scale;

    // カプセルの中心軸ベクトル
    float3 cap_dir = normalize(cap_pos1 - cap_pos2);
    VECTOR cap_v1  = cast(cap_pos1 - (cap_dir * cap_radius));
    VECTOR cap_v2  = cast(cap_pos2 + (cap_dir * cap_radius));

    // ボックスのローカル空間でカプセルを扱う
    matrix inv_box_mat    = inverse(box_mat);
    float3 local_cap_pos1 = mul(float4(cast(cap_v1), 1), inv_box_mat).xyz;
    float3 local_cap_pos2 = mul(float4(cast(cap_v2), 1), inv_box_mat).xyz;

    // AABBとカプセル軸の最近接点を求める
    float3 box_min = -half_size / float3{scale_x, scale_y, scale_z};
    float3 box_max = half_size / float3{scale_x, scale_y, scale_z};

    // カプセルの線分とAABBの最近接点を計算
    auto clamp_point = [](float3 point, float3 min_p, float3 max_p) -> float3 {
        return float3{
            std::max(min_p.x, std::min(max_p.x, point.x)), std::max(min_p.y, std::min(max_p.y, point.y)), std::max(min_p.z, std::min(max_p.z, point.z))};
    };

    // カプセルの線分上の点tを探す (0 <= t <= 1)
    float3 seg_dir = local_cap_pos2 - local_cap_pos1;

    float min_dist_sq        = FLT_MAX;
    float3 closest_box_point = box_pos;
    float3 closest_seg_point = cap_pos1;

    // 簡易的な判定: カプセルの両端点とAABBの最近接点で判定
    for(float t = 0.0f; t <= 1.0f; t += 0.1f)
    {
        float3 seg_point = local_cap_pos1 + seg_dir * t;
        float3 clamped   = clamp_point(seg_point, box_min, box_max);
        float3 diff      = seg_point - clamped;
        float dist_sq    = dot(diff, diff);

        if(dist_sq < min_dist_sq)
        {
            min_dist_sq       = dist_sq;
            closest_box_point = mul(float4(clamped, 1), box_mat).xyz;
            closest_seg_point = mul(float4(seg_point, 1), box_mat).xyz;
        }
    }

    float min_dist = sqrt(min_dist_sq);

    // 衝突判定
    if(min_dist < cap_radius)
    {
        float3 vec = closest_seg_point - closest_box_point;
        float len  = length(vec);

        if(abs(len) <= abs(len) * FLT_EPSILON)
        {
            vec = {0, 0, 1};
        }

        float3 vs = normalize(vec) * cap_radius;
        vec       = vec - vs;

        info.push_         = vec;
        info.hit_          = true;
        info.hit_position_ = (closest_seg_point + closest_box_point) * 0.5f;
    }

    return info;
}

//! @brief Capsule VS Box
//! @param col1 Capsuleコリジョン
//! @param col2 Box コリジョン
//! @return 当たり情報
ComponentCollision::HitInfo ComponentCollision::isHit(ComponentCollisionCapsulePtr col1, ComponentCollisionBoxPtr col2)
{
    auto hit  = isHit(col2, col1);
    hit.push_ = -hit.push_;   // push方向を反対にする
    return hit;
}

//! @brief Box VS Model
//! @param col1 Boxコリジョン
//! @param col2 Model コリジョン
//! @return 当たり情報
ComponentCollision::HitInfo ComponentCollision::isHit(ComponentCollisionBoxPtr col1, ComponentCollisionModelPtr col2)
{
    ComponentCollision::HitInfo info{};

    // モデルが存在していない
    auto mdl = col2->GetOwner()->GetComponent<ComponentModel>();
    if(mdl == nullptr)
    {
        return info;
    }

    // ボックスのワールドマトリクス取得
    auto box_mat = col1->GetWorldMatrix();

    // ボックスのスケール計算
    float scale_x = length(box_mat.axisVectorX());
    float scale_y = length(box_mat.axisVectorY());
    float scale_z = length(box_mat.axisVectorZ());

    // ボックスのサイズ取得(スケール適用)
    float box_width  = col1->GetWidth();
    float box_height = col1->GetHeight();
    float box_depth  = col1->GetDepth();

    if(col1->attach_node_ < 0)
    {
        box_width  *= scale_x;
        box_height *= scale_y;
        box_depth  *= scale_z;
    }

    float3 half_size = {box_width * 0.5f, box_height * 0.5f, box_depth * 0.5f};

    // ボックスの中心位置(現在と1フレーム前)
    float3 opos{};
    float3 cpos{};

    if(col1->attach_node_ >= 0)
    {
        cpos = box_mat.translate();
        opos = cpos;
    }
    else
    {
        // オブジェクト位置に対するコリジョン(1フレーム前)
        opos = mul(float4(col1->GetTranslate(), 1), col1->GetOwner()->GetOldWorldMatrix()).xyz;

        // オブジェクト位置に対するコリジョン
        cpos = mul(float4(col1->GetTranslate(), 1), col1->GetOwner()->GetWorldMatrix()).xyz;
    }

    // ボックスを球で近似して当たり判定
    // (簡易実装: ボックスの対角線の半分を半径とする)
    float radius = half_size.y;

    // 当たりの確認(カプセル形状で代用)
    {
        // 戻し量
        float3 vh = 0;

        MV1_COLL_RESULT_POLY_DIM hit_poly_dim{};

        float3 ocenter = opos + info.push_;
        float3 ncenter = cpos + info.push_;

        // スピードは一旦無視。カプセルをそのまま利用
        hit_poly_dim = MV1CollCheck_Capsule(mdl->GetModel(), -1, cast(ocenter), cast(ncenter), radius);
        for(int i = 0; i < hit_poly_dim.HitNum; i++)
        {
            SEGMENT_TRIANGLE_RESULT result{};

            VECTOR v1 = cast(ocenter);
            VECTOR v2 = cast(ncenter);
            DxLib::Segment_Triangle_Analyse(
                &v1, &v2, &hit_poly_dim.Dim[i].Position[0], &hit_poly_dim.Dim[i].Position[1], &hit_poly_dim.Dim[i].Position[2], &result);

            float3 line_pos = cast(result.Seg_MinDist_Pos);
            float3 tri_pos  = cast(result.Tri_MinDist_Pos);
            float3 tri_nml  = cast(hit_poly_dim.Dim[i].Normal);

            // ボックスへの戻し方向
            if(HelperLib::Math::NearlyEqual(length(line_pos - tri_pos), 0))
            {
                float3 v = (tri_pos - ncenter);
                v        = v + (tri_nml * radius);

                vh = merge(vh, v);
            }
            else
            {
                float3 vec = normalize(line_pos - tri_pos);
                // 戻し量
                float len = radius - length(line_pos - tri_pos);
                float3 v  = vec * len;
                vh        = merge(vh, v);
            }
        }
        if(hit_poly_dim.HitNum > 0)
        {
            info.push_         += vh;
            info.hit_           = true;
            info.hit_position_ += (cpos - vh);
        }
        MV1CollResultPolyDimTerminate(hit_poly_dim);
    }

    return info;
}

//! @brief Model VS Box
//! @param col1 Modelコリジョン
//! @param col2 Box コリジョン
//! @return 当たり情報
ComponentCollision::HitInfo ComponentCollision::isHit(ComponentCollisionModelPtr col1, ComponentCollisionBoxPtr col2)
{
    auto hit  = isHit(col2, col1);
    hit.push_ = -hit.push_;   // push方向を反対にする
    return hit;
}

//! @brief Sphere VS Capsule
//! @param col1 Sphereコリジョン
//! @param col2 Capsule コリジョン
//! @return 当たり情報
ComponentCollision::HitInfo ComponentCollision::isHit(ComponentCollisionModelPtr col1, ComponentCollisionSpherePtr col2)
{
    auto hit  = isHit(col2, col1);
    hit.push_ = -hit.push_;   // push方向を反対にする
    return hit;
}

//! @brief Model VS Capsule
//! @param col1 Modelコリジョン
//! @param col2 Capsule コリジョン
//! @return 当たり情報
ComponentCollision::HitInfo ComponentCollision::isHit(ComponentCollisionCapsulePtr col1, ComponentCollisionModelPtr col2)
{
    ComponentCollision::HitInfo info{};

    // モデルが存在していない
    auto mdl = col2->GetOwner()->GetComponent<ComponentModel>();
    if(mdl == nullptr)
    {
        return info;
    }

    float3 opos{};
    float3 cpos{};
    float3 opos1{};
    float3 cpos1{};
    auto trans = col1->GetWorldMatrix();

    if(col1->attach_node_ >= 0)
    {
        cpos  = trans.translate();
        cpos1 = normalize((float3&)trans.axisVectorY()) * col1->GetHeight() + cpos;

        opos  = cpos;
        opos1 = cpos1;
    }
    else
    {
        // オブジェクト位置に対するコリジョン(1フレーム前)
        opos = mul(float4(col1->GetTranslate(), 1), col1->GetOwner()->GetOldWorldMatrix()).xyz;

        // オブジェクト位置に対するコリジョン
        cpos = mul(float4(col1->GetTranslate(), 1), col1->GetOwner()->GetWorldMatrix()).xyz;

        // 実際の移動できる量にする
        auto move   = cpos - opos;
        float movey = move.y;
        move.y      = 0;

        if(col1->IsUseGravity())
        {
            move   = col2->checkMovement(opos, move, difficult_to_climb_);
            move.y = movey;
            cpos   = opos + move;

            // 当たりからキャラの位置を求める
            auto rot                      = col1->GetOwner()->GetMatrix();
            rot._41_42_43                 = float3{0.0f, 0.0f, 0.0f};
            auto col1r                    = mul(float4(col1->GetTranslate(), 1), rot).xyz;
            col1->GetOwner()->Translate() = cpos - col1r;
        }

        // その頭の位置
        opos1 = mul(float4(col1->GetTranslate() + float3(0, col1->GetHeight(), 0), 1), col1->GetOwner()->GetOldWorldMatrix()).xyz;
        cpos1 = mul(float4(col1->GetTranslate() + float3(0, col1->GetHeight(), 0), 1), col1->GetOwner()->GetWorldMatrix()).xyz;
    }

    float scale = 1.0f;
    if(col1->attach_node_ < 0)
    {
        auto mat = col1->GetWorldMatrix();
        float sx = length(mat.axisVectorX());
        float sy = length(mat.axisVectorY());
        float sz = length(mat.axisVectorZ());
        scale    = (sx + sy + sz) / 3.0f;
    }

    float radius = col1->GetRadius() * scale;

    MV1_COLL_RESULT_POLY hit_poly{};
    float3 bottom = cpos;    //-float3{ 0, col1->GetRadius() * scale, 0 };
    float3 top    = opos1;   // bottom + float3{ 0, col1->GetRadius() * scale * 2, 0 };
    hit_poly      = MV1CollCheck_Line(mdl->GetModel(), -1, cast(top), cast(bottom));

    if(hit_poly.HitFlag != 0)
    {
        float3 pos = cast(hit_poly.HitPosition);

        // 半径分押し戻し
        float3 vec = pos - cpos;

        // 一旦1つ目の当たりだけで返してみる
        // このpush_は、調べたほうの押し戻し方向100%で作成する
        info.push_         = vec;
        info.hit_          = true;
        info.hit_position_ = pos;
    }
#if 0
	// 下の球当たりの確認
	{
		// 戻し量
		float3 vh = 0;

		MV1_COLL_RESULT_POLY_DIM hit_poly_dim{};

		float3 oc = opos + float3(0, radius, 0);
		float3 cc = cpos + float3(0, radius, 0) - now_gravity_;

		//DrawCapsule3D( cast( oc ), cast( cc ), radius, 10, GetColor( 0, 0, 255 ), GetColor( 0, 0, 255 ), FALSE );

		hit_poly_dim = MV1CollCheck_Capsule(mdl->GetModel(), -1, cast(oc), cast(cc), radius);
		for (int i = 0; i < hit_poly_dim.HitNum; i++)
		{
			SEGMENT_TRIANGLE_RESULT result{};

			VECTOR v1 = cast(oc);
			VECTOR v2 = cast(cc);
			DxLib::Segment_Triangle_Analyse(&v1,
				&v2,
				&hit_poly_dim.Dim[i].Position[0],
				&hit_poly_dim.Dim[i].Position[1],
				&hit_poly_dim.Dim[i].Position[2],
				&result);

			float3 line_pos = cast(result.Seg_MinDist_Pos);
			float3 tri_pos = cast(result.Tri_MinDist_Pos);

			// カプセルへの戻し方向
			if (HelperLib::Math::NearlyEqual(length(line_pos - tri_pos), 0))
			{
				float3 v = (tri_pos - cpos);
				vh = merge(vh, v);
			}
			else
			{
				float3 vec = normalize(line_pos - tri_pos);
				// 戻し量
				float  len = radius - length(line_pos - tri_pos);
				float3 v = vec * len;
				vh = merge(vh, v);
			}
		}

		if (hit_poly_dim.HitNum > 0)
		{
			info.push_ += vh;
			info.hit_ = true;
			info.hit_position_ += cpos - vh;
		}
		MV1CollResultPolyDimTerminate(hit_poly_dim);
	}
#endif

    {
        // 戻し量
        float3 vh = 0;

        MV1_COLL_RESULT_POLY_DIM hit_poly_dim{};

        // カプセルの軸方向ベクトルを取得
        float3 capsule_axis = normalize(trans.axisY());
        // カプセルの軸方向に沿って半径分オフセット
        float3 oc = opos1 - capsule_axis * radius + info.push_;
        float3 cc = opos + capsule_axis * radius * 2.0f + info.push_;

        //DrawCapsule3D(cast(oc), cast(cc), radius, 10, GetColor(0, 0, 255), GetColor(0, 0, 255), FALSE);
        hit_poly_dim = MV1CollCheck_Capsule(mdl->GetModel(), -1, cast(oc), cast(cc), radius);
        for(int i = 0; i < hit_poly_dim.HitNum; i++)
        {
            SEGMENT_TRIANGLE_RESULT result{};

            VECTOR v1 = cast(oc);
            VECTOR v2 = cast(cc);
            DxLib::Segment_Triangle_Analyse(
                &v1, &v2, &hit_poly_dim.Dim[i].Position[0], &hit_poly_dim.Dim[i].Position[1], &hit_poly_dim.Dim[i].Position[2], &result);

            float3 line_pos = cast(result.Seg_MinDist_Pos);
            float3 tri_pos  = cast(result.Tri_MinDist_Pos);

            // カプセルへの戻し方向
            if(HelperLib::Math::NearlyEqual(length(line_pos - tri_pos), 0))
            {
                float3 v = (tri_pos - cpos1);
                vh       = merge(vh, v);
            }
            else
            {
                float3 vec = normalize(line_pos - tri_pos);
                // 戻し量
                float len = radius - length(line_pos - tri_pos);
                float3 v  = vec * len;
                vh        = merge(vh, v);
            }
        }
        if(hit_poly_dim.HitNum > 0)
        {
            info.push_         += vh;
            info.hit_           = true;
            info.hit_position_ += (cpos - vh);
        }
        MV1CollResultPolyDimTerminate(hit_poly_dim);
    }

    // 当たりの確認(通常カプセル)
    float3 nyopos = opos;
    float3 nycpos = cpos;
    nyopos.y      = 0;
    nycpos.y      = 0;
    if(!HelperLib::Math::NearlyEqual(length(nyopos - nycpos), 0))
    {
        // 戻し量
        float3 vh = 0;

        MV1_COLL_RESULT_POLY_DIM hit_poly_dim{};

        float3 bottomx = cpos + info.push_ + float3(0, radius, 0);
        float3 topx    = cpos1 - float3(0, radius, 0);

        // DrawCapsule3D( cast( topx ), cast( bottomx ), radius, 10, GetColor( 0, 0, 255 ), GetColor( 0, 0, 255 ), FALSE );

        hit_poly_dim = MV1CollCheck_Capsule(mdl->GetModel(), -1, cast(topx), cast(bottomx), radius);
        for(int i = 0; i < hit_poly_dim.HitNum; i++)
        {
            SEGMENT_TRIANGLE_RESULT result{};

            VECTOR v1 = cast(topx);
            VECTOR v2 = cast(bottomx);
            DxLib::Segment_Triangle_Analyse(
                &v1, &v2, &hit_poly_dim.Dim[i].Position[0], &hit_poly_dim.Dim[i].Position[1], &hit_poly_dim.Dim[i].Position[2], &result);

            float3 line_pos = cast(result.Seg_MinDist_Pos);
            float3 tri_pos  = cast(result.Tri_MinDist_Pos);

            // カプセルへの戻し方向
            if(HelperLib::Math::NearlyEqual(length(line_pos - tri_pos), 0))
            {
                float3 v = (tri_pos - cpos);
                vh       = merge(vh, v);
            }
            else
            {
                float3 vec = normalize(line_pos - tri_pos);
                // 戻し量
                float len = radius - length(line_pos - tri_pos);
                float3 v  = vec * len;
                vh        = merge(vh, v);
            }
        }
        if(hit_poly_dim.HitNum > 0)
        {
            info.push_         += vh;
            info.hit_           = true;
            info.hit_position_ += (cpos - vh);
        }
        MV1CollResultPolyDimTerminate(hit_poly_dim);
    }

    return info;
}

//! @brief Sphere VS Capsule
//! @param col1 Sphereコリジョン
//! @param col2 Capsule コリジョン
//! @return 当たり情報
ComponentCollision::HitInfo ComponentCollision::isHit(ComponentCollisionSpherePtr col1, ComponentCollisionLinePtr col2)
{
    auto hit  = isHit(col2, col1);
    hit.push_ = -hit.push_;   // push方向を反対にする
    return hit;
}

ComponentCollision::HitInfo ComponentCollision::isHit(ComponentCollisionCapsulePtr col1, ComponentCollisionLinePtr col2)
{
    auto hit  = isHit(col2, col1);
    hit.push_ = -hit.push_;   // push方向を反対にする
    return hit;
}

// Line(col1)とSphere(col2)の当たりをチェックします
ComponentCollision::HitInfo ComponentCollision::isHit(ComponentCollisionLinePtr col1, ComponentCollisionSpherePtr col2)
{
    // 当たり情報
    ComponentCollision::HitInfo info{};

    // ラインの情報
    auto line_owner = col1->GetOwner();
    auto line_comp  = line_owner->GetComponent<ComponentCollisionLine>();
    auto line       = line_comp->GetWorldLine();

    // 球の情報
    auto sphere_owner = col2->GetOwner();
    if(sphere_owner == nullptr)
    {
        return info;
    }

    auto sphere = sphere_owner->GetComponent<ComponentCollisionSphere>();
    if(sphere == nullptr)
    {
        return info;
    }

    auto pos     = sphere->GetWorldMatrix().translate();
    float radius = sphere->GetRadius();

    SEGMENT_POINT_RESULT result;

    // ライン情報と点情報を用意する
    VECTOR start  = cast(line[0]);
    VECTOR end    = cast(line[1]);
    VECTOR center = cast(pos);

    // ラインと点により分析する
    Segment_Point_Analyse(&start, &end, &center, &result);

    // 点にライン上で一番近い位置を割り出す
    auto point = cast(result.Seg_MinDist_Pos);
    // その位置から点までの距離を割り出す
    float len = length(point - pos);

    // その距離が半径よりも小さければ当たっている
    if(len <= radius)
    {
        auto vec           = normalize(line[0] - line[1]);
        info.hit_          = true;
        info.hit_position_ = point + vec * acos(len / radius) / 0.5f * DX_PI_F;
    }

    // 判定結果を返す
    return info;
}

// Line(col1)とCapsule(col2)の当たりをチェックします
ComponentCollision::HitInfo ComponentCollision::isHit(ComponentCollisionLinePtr col1, ComponentCollisionCapsulePtr col2)
{
    ComponentCollision::HitInfo info{};

    // ラインの情報
    auto line_owner = col1->GetOwner();
    auto line_comp  = line_owner->GetComponent<ComponentCollisionLine>();
    auto line       = line_comp->GetWorldLine();

    // カプセルの情報
    auto capsule_owner = col2->GetOwner();
    if(capsule_owner == nullptr)
    {
        return info;
    }

    auto capsule = capsule_owner->GetComponent<ComponentCollisionCapsule>();
    if(capsule == nullptr)
    {
        return info;
    }

    // カプセルの状態を分析
    auto mat     = capsule->GetWorldMatrix();
    auto pos     = mat.translate();
    float height = capsule->GetHeight();
    float radius = capsule->GetRadius();
    auto pos1    = pos + height * mat.axisY();

    SEGMENT_SEGMENT_RESULT result;

    // ライン情報とカプセルの縦ラインを互いにチェックする
    VECTOR l1_start = cast(line[0]);
    VECTOR l1_end   = cast(line[1]);
    VECTOR l2_start = cast(pos);
    VECTOR l2_end   = cast(pos1);

    // ラインどうしの状態を分析
    Segment_Segment_Analyse(&l1_start, &l1_end, &l2_start, &l2_end, &result);

    // ライン上の一番近い点を割り出し、その距離を確認する
    auto point     = cast(result.SegA_MinDist_Pos);
    auto cap_point = cast(result.SegB_MinDist_Pos);
    float len      = length(point - cap_point);

    // 片方はラインなのであくまで半径よりもこれが小さいかで
    // 当たったかどうかが判定できる
    if(len <= radius)
    {
        // 当たったら情報を入れておく
        auto vec           = normalize(line[0] - line[1]);
        info.hit_          = true;
        info.hit_position_ = point + vec * acos(len / radius) / 0.5f * DX_PI_F;
    }

    // 判定結果を返す
    return info;
}

// ライン(col1)とモデル(col2)での当たりを判定する
ComponentCollision::HitInfo ComponentCollision::isHit(ComponentCollisionLinePtr col1, ComponentCollisionModelPtr col2)
{
    ComponentCollision::HitInfo info{};

    // ライン情報
    auto line_owner = col1->GetOwner();
    auto line_comp  = line_owner->GetComponent<ComponentCollisionLine>();
    auto line       = line_comp->GetWorldLine();

    // モデル情報
    auto model_owner = col2->GetOwner();
    if(model_owner == nullptr)
    {
        return info;
    }

    auto model = model_owner->GetComponent<ComponentModel>();
    if(model == nullptr)
    {
        return info;
    }

    MV1_COLL_RESULT_POLY hit_poly{};

    // MV1とラインのチェック関数を呼び出す
    hit_poly = MV1CollCheck_Line(model->GetModel(), -1, cast(line[0]), cast(line[1]));

    // 当たったかどうかのフラグを設定し、当たった位置も入れておく
    info.hit_          = hit_poly.HitFlag;
    info.hit_position_ = float3(hit_poly.HitPosition.x, hit_poly.HitPosition.y, hit_poly.HitPosition.z);

    return info;
}

//! @brief Capsule VS Model
//! @param col1 Capsuleコリジョン
//! @param col2 Model コリジョン
//! @return 当たり情報
ComponentCollision::HitInfo ComponentCollision::isHit(ComponentCollisionModelPtr col1, ComponentCollisionCapsulePtr col2)
{
    auto hit  = isHit(col2, col1);
    hit.push_ = -hit.push_;   // push方向を反対にする
    return hit;
}

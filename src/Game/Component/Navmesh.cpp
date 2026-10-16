//===========================================================================
//! @file Navmesh.cpp
//! @brief ナビメッシュコンポーネント
//===========================================================================
#include "Navmesh.h"
#include <System/Object.h>
#include <System/Utils/HelperLib.h>
#include <RecastNavigation/Detour/DetourAlloc.h>
#include <RecastNavigation/Detour/DetourNavMeshBuilder.h>
#include <RecastNavigation/DebugUtils/DetourDebugDraw.h>
#include <RecastNavigation/DebugUtils/DxLibDebugDraw.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <Game/Component/Enemy/EnemyMove.h>

struct NavMeshSetHeader
{
    int magic_;                // バージョン更新しないため、使わない
    int version_;              // バージョン更新しないため、使わない
    int num_tiles_;            // タイル数
    dtNavMeshParams params_;   // ナビメッシュパラメータ
};
struct NavMeshTileHeader
{
    dtTileRef tile_ref_;   // タイル参照
    int data_size_;        // データサイズ
};

//! @brief 初期化
void Navmesh::Init()
{
    Super::Init();

    LoadNavmesh("data/Game/Navmesh/solo_navmesh.bin");

    // ナビメッシュのメモリー確保と初期化
    query_ = dtAllocNavMeshQuery();
    query_->init(nav_mesh_, 2048);

    // 探索範囲設定
    if(auto col = GetOwner()->GetComponent<ComponentCollisionCapsule>())
    {
        float r              = col->GetRadius();
        float h              = col->GetHeight();
        search_half_extents_ = {r, h, r};
    }
    // 移動速度取得
    if(auto enemy_move = GetOwner()->GetComponent<EnemyMove>())
    {
        move_speed_ = enemy_move->GetSpeed();
    }

    SetProc("Navmesh:Update", [this]() { Update(); }, ProcTiming::Update, static_cast<ProcPriority>(1000));
}

//! @brief 更新
void Navmesh::Update()
{
    Super::Update();
    if(!nav_mesh_ || !query_ || !use_navmesh_)
    {
        return;
    }
    // ステージオブジェクト取得
    if(auto stage = Scene::Object::Get<Object>("Stage"))
    {
        navmesh_anchor_ = stage;
    }
    float3 goal{};

    if(auto t = target_.lock())
    {
        goal = t->GetTranslate();
    }
    else if(has_target_)
    {
        goal = target_pos_;
    }
    else
    {
        // ターゲットない
        return;
    }

    // 一定間隔で経路再計算
    repath_timer_ += GetDeltaTime();
    if((repath_timer_ >= repath_interval_ || path_points_.empty()))
    {
        repath_timer_ = 0.0f;
        RebuildPath();

        if(path_points_.empty())
        {
            follow_ref_ = 0;
        }
    }

    if(auto enemy_move = GetOwner()->GetComponent<EnemyMove>())
    {
        move_speed_ = enemy_move->GetSpeed();
    }

    // 近すぎるポイントはスキップ
    float3 pos = GetOwner()->GetTranslate();
    while(current_point_ < (int)path_points_.size())
    {
        float3 target_point = path_points_[current_point_];
        float3 dir          = target_point - pos;
        dir.y               = 0.0f;
        float len           = sqrtf(dot(dir, dir));

        if(len > 0.1f)
        {
            break;
        }
        current_point_++;
    }

    if(path_points_.empty() || current_point_ >= (int)path_points_.size())
    {
        if(auto enemy_move = GetOwner()->GetComponent<EnemyMove>())
        {
            enemy_move->SetVelocity({0, 0, 0});
        }
        return;
    }

    float3 target_point = path_points_[current_point_];
    float3 dir          = target_point - pos;
    dir.y               = 0.0f;

    float dist_to_target = sqrtf(dot(dir, dir));

    // ターゲットポイントに到達したかチェック
    if(dist_to_target < 0.3f)
    {
        current_point_++;
        if(current_point_ >= (int)path_points_.size())
        {
            // 最終地点に到達したら停止
            if(auto enemy_move = GetOwner()->GetComponent<EnemyMove>())
            {
                enemy_move->SetVelocity({0, 0, 0});
            }
            return;
        }
        target_point   = path_points_[current_point_];
        dir            = target_point - pos;
        dir.y          = 0.0f;
        dist_to_target = sqrtf(dot(dir, dir));
    }

    // 方向を正規化
    if(dist_to_target > FLT_EPSILON)
    {
        dir = dir / dist_to_target;
    }
    else
    {
        // 完全に同じ位置の場合は停止
        if(auto enemy_move = GetOwner()->GetComponent<EnemyMove>())
        {
            enemy_move->SetVelocity({0, 0, 0});
        }
        return;
    }

    // パスから外れていないかチェック（オプション）
    // ナビメッシュ上の最寄り位置を取得して補正
    matrix anchor_m = matrix::identity();
    if(auto a = navmesh_anchor_.lock())
    {
        anchor_m = a->GetWorldMatrix();
    }
    matrix inv_anchor = inverse(anchor_m);

    float nav_pos[3] = {};
    {
        float3 convert_nav_pos = TransformPoint(inv_anchor, pos);
        convert_nav_pos        = RHtoLHPoint(convert_nav_pos);
        convert_nav_pos        = ScalePoint(convert_nav_pos, 1.0f / navmesh_unit_scale_);
        nav_pos[0]             = convert_nav_pos.x;
        nav_pos[1]             = convert_nav_pos.y;
        nav_pos[2]             = convert_nav_pos.z;
    }

    // 近傍ポリゴンをチェック（パスから大きく外れた場合の補正用）
    float ext[3] = {search_half_extents_.x / navmesh_unit_scale_, search_half_extents_.y / navmesh_unit_scale_, search_half_extents_.z / navmesh_unit_scale_};
    float nearest_point[3];
    dtPolyRef ref = 0;
    if(dtStatusSucceed(query_->findNearestPoly(nav_pos, ext, &filter_, &ref, nearest_point)))
    {
        // ナビメッシュ上の位置に変換
        float3 nearest_pos{nearest_point[0], nearest_point[1], nearest_point[2]};
        nearest_pos = ScalePoint(nearest_pos, navmesh_unit_scale_);
        nearest_pos = RHtoLHPoint(nearest_pos);
        nearest_pos = TransformPoint(anchor_m, nearest_pos);

        // 現在位置とナビメッシュ上の位置の差が大きい場合は補正
        float3 diff_to_navmesh = nearest_pos - pos;
        diff_to_navmesh.y      = 0;
        float dist_to_navmesh  = sqrtf(dot(diff_to_navmesh, diff_to_navmesh));

        if(dist_to_navmesh > 0.5f)
        {
            // パスから大きく外れているので、ナビメッシュに戻る方向を混ぜる
            float3 correction_dir = normalize(diff_to_navmesh);
            dir                   = normalize(dir * 0.7f + correction_dir * 0.3f);
        }
    }

    if(auto enemy_move = GetOwner()->GetComponent<EnemyMove>())
    {
        enemy_move->SetVelocity(dir);
    }
}

//! @brief 描画
void Navmesh::LateDraw()
{
    Super::LateDraw();
    if(debug_draw_navmesh_)
    {
        // ナビメッシュ描画
        DrawNavmeshDebug();

        if(path_points_.size() >= 1)
        {
            for(int i = 1; i < (int)path_points_.size(); i++)
            {
                DrawLine3D(cast(path_points_[i - 1]), cast(path_points_[i]), GetColor(0, 255, 255));
            }
            for(const auto& p : path_points_)
            {
                DrawSphere3D(cast(p), 0.1f, 12, GetColor(255, 0, 255), GetColor(255, 0, 255), true);
            }
        }
        // ターゲットポイントを描画
        if(path_points_.size() > 0)
        {
            DrawSphere3D(
                cast(path_points_[std::min(current_point_, (int)path_points_.size() - 1)]), 0.3f, 12, GetColor(255, 255, 0), GetColor(255, 255, 0), true);
        }
    }
}

//! @brief 終了
void Navmesh::Exit()
{
    Super::Exit();
    // ナビメッシュ解放
    if(query_)
    {
        dtFreeNavMeshQuery(query_);
        query_ = nullptr;
    }
    if(nav_mesh_)
    {
        dtFreeNavMesh(nav_mesh_);
        nav_mesh_ = nullptr;
    }
}

//! @brief GUI表示処理
void Navmesh::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    ImGui::Separator();
    if(ImGui::TreeNode("Navmesh"))
    {
        if(ImGui::Button(u8"削除"))
        {
            GetOwner()->RemoveComponent(shared_from_this());
        }
        ImGui::Text("path points: %d", (int)path_points_.size());
        ImGui::Checkbox("Draw NavMesh", &debug_draw_navmesh_);
        ImGui::Checkbox("Use Navmesh", &use_navmesh_);

        ImGui::TreePop();
    }
    ImGui::End();
}

//! @brief パスを再計算
void Navmesh::RebuildPath()
{
    float3 end = float3{0.0f, 0.0f, 0.0f};

    if(has_target_)
    {
        end = target_pos_;
    }
    else
    {
        if(auto t = target_.lock())
        {
            end = t->GetTranslate();
        }
    }
    // パスを計算
    float3 start = GetOwner()->GetTranslate();
    ComputePath(start, end);
}

//! @brief パス検索の初期化
bool Navmesh::PathSearch(
    const float3& start_pos, const float3& end_pos, dtPolyRef& start_ref, dtPolyRef& end_ref, float* clamped_start, float* clamped_end) const
{
    if(!nav_mesh_ || !query_)
    {
        return false;
    }

    // アンカー基準に変換
    matrix anchor_m = matrix::identity();
    float3 start    = start_pos;
    float3 end      = end_pos;

    if(auto achor = navmesh_anchor_.lock())
    {
        anchor_m = achor->GetWorldMatrix();
        start    = TransformPoint(inverse(anchor_m), start);
        end      = TransformPoint(inverse(anchor_m), end);
    }

    // LH→RH + スケール縮小
    start = RHtoLHPoint(start);
    end   = RHtoLHPoint(end);
    start = ScalePoint(start, 1.0f / navmesh_unit_scale_);
    end   = ScalePoint(end, 1.0f / navmesh_unit_scale_);

    float start_p[3] = {start.x, start.y, start.z};
    float end_p[3]   = {end.x, end.y, end.z};

    float ext_start[3] = {
        search_half_extents_.x / navmesh_unit_scale_, search_half_extents_.y / navmesh_unit_scale_, search_half_extents_.z / navmesh_unit_scale_};
    float ext_end[3] = {ext_start[0], ext_start[1], ext_start[2]};

    // 開始位置に最も近いポリゴンを取得
    if(dtStatusFailed(query_->findNearestPoly(start_p, ext_start, &filter_, &start_ref, clamped_start)) || start_ref == 0)
    {
        // 敵がナビメッシュのマップから外れている場合もあるため
        // 探索範囲を広げて再検索する
        for(int i = 0; i < 3; i++)
        {
            if(dtStatusSucceed(query_->findNearestPoly(start_p, ext_start, &filter_, &start_ref, clamped_start)) && start_ref != 0)
            {
                break;
            }
            ext_start[0] *= 2;
            ext_start[1] *= 2;
            ext_start[2] *= 2;
        }
    }

    // プレイヤ位置に最も近いポリゴンを取得
    if(dtStatusFailed(query_->findNearestPoly(end_p, ext_end, &filter_, &end_ref, clamped_end)) || end_ref == 0)
    {
        // プレイヤがナビメッシュのマップから外れている場合もあるため
        // 探索範囲を広げて再検索する
        for(int i = 0; i < 6; i++)
        {
            if(dtStatusSucceed(query_->findNearestPoly(end_p, ext_end, &filter_, &end_ref, clamped_end)) && end_ref != 0)
            {
                break;
            }
            ext_end[0] *= 2;
            ext_end[1] *= 2;
            ext_end[2] *= 2;
        }
    }

    if(start_ref == 0 || end_ref == 0)
    {
        return false;
    }

    return true;
}

//! @brief パスを計算
//! @param start_position 開始位置
//! @param end_position 終了位置
//! @retval true 計算成功
//! @retval false 計算失敗
bool Navmesh::ComputePath(float3& start_position, float3& end_position)
{
    path_points_.clear();
    current_point_      = 0;
    dtPolyRef start_ref = 0, end_ref = 0;
    float clamped_start[3] = {0, 0, 0};
    float clamped_end[3]   = {0, 0, 0};

    if(!PathSearch(start_position, end_position, start_ref, end_ref, clamped_start, clamped_end))
    {
        return false;
    }

    int polys = 0;

    // ポリゴン経路探索
    query_->findPath(start_ref, end_ref, clamped_start, clamped_end, &filter_, polys_, &polys, MAX_POLYS_);

    // ファンネルアルゴリズムで経路を直線化
    int straight = 0;
    query_->findStraightPath(
        clamped_start, clamped_end, polys_, polys, straight_pts_, straight_flags_, straight_refs_, &straight, MAX_STRAIGHT_, DT_STRAIGHTPATH_ALL_CROSSINGS);

    // アンカー行列を取得
    matrix anchor_m = matrix::identity();
    if(auto anchor = navmesh_anchor_.lock())
    {
        anchor_m = anchor->GetWorldMatrix();
    }

    // 結果を格納
    path_points_.reserve((size_t)straight);
    float3 prev{};
    for(int i = 0; i < straight; i++)
    {
        float3 p{straight_pts_[i * 3 + 0], straight_pts_[i * 3 + 1], straight_pts_[i * 3 + 2]};
        if(straight_refs_[i] != 0)
        {
            float closest[3];
            float pos3[3] = {p.x, p.y, p.z};

            // ポリゴン上の最近接点を取得
            bool over;
            query_->closestPointOnPoly(straight_refs_[i], pos3, closest, &over);

            p.x = closest[0];
            p.y = closest[1];
            p.z = closest[2];
        }
        p = ScalePoint(p, navmesh_unit_scale_);   // スケール戻し
        p = RHtoLHPoint(p);                       // 座標系戻し
        p = TransformPoint(anchor_m, p);          // アンカー座標系からワールド座標系へ

        path_points_.push_back(p);
        prev = p;
    }
    return !path_points_.empty();
}

//! @brief 最寄りのナビメッシュ上の位置を取得
//! @param pos 検索位置
//! @return 最寄りの位置
float3 Navmesh::GetNearestPos(float3 pos) const
{
    float3 result;
    return TryGetNearestPos(pos, result) ? result : pos;
}

bool Navmesh::TryGetNearestPos(float3 pos, float3& result) const
{
    if(!nav_mesh_ || !query_)
    {
        return false;
    }

    // アンカー基準に変換
    float3 nav_pos  = pos;
    matrix anchor_m = matrix::identity();
    if(auto achor = navmesh_anchor_.lock())
    {
        anchor_m = achor->GetWorldMatrix();
        nav_pos  = TransformPoint(inverse(anchor_m), nav_pos);
    }

    // LH→RH + スケール縮小
    nav_pos = RHtoLHPoint(nav_pos);
    nav_pos = ScalePoint(nav_pos, 1.0f / navmesh_unit_scale_);

    float ext[3]  = {search_half_extents_.x / navmesh_unit_scale_, search_half_extents_.y / navmesh_unit_scale_, search_half_extents_.z / navmesh_unit_scale_};
    dtPolyRef ref = 0;
    float nearest_[3] = {};
    // 近いポリゴンを探す
    if(dtStatusSucceed(query_->findNearestPoly((const float*)&nav_pos, ext, &filter_, &ref, nearest_)) && ref != 0)
    {
        float3 nearest_pos = float3{nearest_[0], nearest_[1], nearest_[2]};
        result             = ScalePoint(nearest_pos, navmesh_unit_scale_);
        result             = RHtoLHPoint(result);
        result             = TransformPoint(anchor_m, result);
        return true;
    }
    return false;
}

//! @brief ナビメッシュ上の距離を取得
float Navmesh::GetNavmeshDistance(const float3& target_pos)
{
    return GetNavmeshDistance(GetOwner()->GetTranslate(), target_pos);
}

float Navmesh::GetNavmeshDistance(const float3& start_pos, const float3& target_pos)
{
    if(!nav_mesh_ || !query_)
    {
        return FLT_MAX;
    }

    dtPolyRef start_ref = 0, end_ref = 0;
    float clamped_start[3] = {0, 0, 0};
    float clamped_end[3]   = {0, 0, 0};

    if(!PathSearch(start_pos, target_pos, start_ref, end_ref, clamped_start, clamped_end))
    {
        return FLT_MAX;
    }

    int polys = 0;
    // ポリゴン経路探索
    if(dtStatusFailed(query_->findPath(start_ref, end_ref, clamped_start, clamped_end, &filter_, polys_, &polys, MAX_POLYS_)) || polys == 0)
    {
        return FLT_MAX;
    }

    int straight = 0;
    query_->findStraightPath(
        clamped_start, clamped_end, polys_, polys, straight_pts_, straight_flags_, straight_refs_, &straight, MAX_STRAIGHT_, DT_STRAIGHTPATH_ALL_CROSSINGS);

    if(straight == 0)
    {
        return FLT_MAX;
    }

    float total_distance = 0.0f;
    float3 prev_p{straight_pts_[0], straight_pts_[1], straight_pts_[2]};

    for(int i = 1; i < straight; i++)
    {
        float3 current_p{straight_pts_[i * 3 + 0], straight_pts_[i * 3 + 1], straight_pts_[i * 3 + 2]};
        float3 diff     = current_p - prev_p;
        float dist      = sqrtf(dot(diff, diff));
        total_distance += dist;
        prev_p          = current_p;
    }

    return total_distance * navmesh_unit_scale_;
}

bool Navmesh::IsInNavmesh(float3 start_pos, float3 end_pos)
{
    if(!nav_mesh_ || !query_)
    {
        return false;
    }
    // アンカー基準に変換
    matrix anchor_m = matrix::identity();
    float3 start    = start_pos;
    float3 end      = end_pos;

    if(auto achor = navmesh_anchor_.lock())
    {
        anchor_m = achor->GetWorldMatrix();
        start    = TransformPoint(inverse(anchor_m), start);
        end      = TransformPoint(inverse(anchor_m), end);
    }

    // LH→RH + スケール縮小
    start = RHtoLHPoint(start);
    end   = RHtoLHPoint(end);
    start = ScalePoint(start, 1.0f / navmesh_unit_scale_);
    end   = ScalePoint(end, 1.0f / navmesh_unit_scale_);

    float start_pos_[3] = {start.x, start.y, start.z};
    float end_pos_[3]   = {end.x, end.y, end.z};
    dtPolyRef start_ref = 0, end_ref = 0;
    float clamped_start[3] = {0, 0, 0};
    float clamped_end[3]   = {0, 0, 0};
    float ext_start[3]     = {
        search_half_extents_.x * 0.1f / navmesh_unit_scale_, search_half_extents_.y / navmesh_unit_scale_, search_half_extents_.z * 0.1f / navmesh_unit_scale_};
    float ext_end[3] = {ext_start[0], ext_start[1], ext_start[2]};
    // 開始位置に最も近いポリゴンを取得
    query_->findNearestPoly(start_pos_, ext_start, &filter_, &start_ref, clamped_start);
    if(start_ref == 0)
    {
        return false;
    }

    // 元の位置からずれたらナビメッシュ外
    float diff_x = clamped_start[0] - start_pos_[0];
    float diff_z = clamped_start[2] - start_pos_[2];
    if(diff_x * diff_x + diff_z * diff_z > ext_start[0] * ext_start[0])
    {
        return false;
    }

    query_->findNearestPoly(end_pos_, ext_end, &filter_, &end_ref, clamped_end);
    if(end_ref == 0)
    {
        return false;
    }

    int polys = 0;
    query_->findPath(start_ref, end_ref, clamped_start, clamped_end, &filter_, polys_, &polys, MAX_POLYS_);
    if(polys == 0 || polys_[polys - 1] != end_ref)
    {
        return false;
    }

    return true;
}

//! @brief ナビメッシュをロード
//! @param path ファイルパス
void Navmesh::LoadNavmesh(const char* path)
{
    FILE* fp;
    if(fopen_s(&fp, path, "rb") != 0)
    {
        return;
    }

    NavMeshSetHeader header;
    size_t read_len = fread(&header, sizeof(NavMeshSetHeader), 1, fp);
    if(read_len != 1)
    {
        fclose(fp);
        return;
    }
    dtNavMesh* mesh = dtAllocNavMesh();
    if(!mesh)
    {
        fclose(fp);
        return;
    }
    dtStatus status = mesh->init(&header.params_);
    if(dtStatusFailed(status))
    {
        fclose(fp);
        return;
    }

    // タイルデータ読み込み
    for(int i = 0; i < header.num_tiles_; i++)
    {
        NavMeshTileHeader tile_header;
        read_len = fread(&tile_header, sizeof(tile_header), 1, fp);
        if(read_len != 1)
        {
            fclose(fp);
            return;
        }

        if(!tile_header.tile_ref_ || !tile_header.data_size_)
        {
            break;
        }

        unsigned char* data = (unsigned char*)dtAlloc(tile_header.data_size_, DT_ALLOC_PERM);
        if(!data)
        {
            break;
        }

        // データを初期化
        memset(data, 0, tile_header.data_size_);
        read_len = fread(data, tile_header.data_size_, 1, fp);
        if(read_len != 1)
        {
            dtFree(data);
            fclose(fp);
            return;
        }

        mesh->addTile(data, tile_header.data_size_, DT_TILE_FREE_DATA, tile_header.tile_ref_, 0);
    }

    fclose(fp);

    nav_mesh_ = mesh;
}

//! @brief NavMesh デバッグ描画
void Navmesh::DrawNavmeshDebug()
{
    if(!nav_mesh_)
    {
        return;
    }
    DxLibDebugDraw dd;
    MATRIX ident = MGetIdent();
    MATRIX world = MGetIdent();

    if(auto a = navmesh_anchor_.lock())
    {
        world = cast(a->GetWorldMatrix());
    }

    MATRIX scale = MGetScale({navmesh_unit_scale_, navmesh_unit_scale_, navmesh_unit_scale_});
    world        = MMult(scale, world);
    world        = MMult(cast(RHtoLHMatrix()), world);
    SetTransformToWorld(&world);
    duDebugDrawNavMesh(&dd, *nav_mesh_, DU_DRAWNAVMESH_COLOR_TILES);
    SetTransformToWorld(&ident);
}

//! @brief ターゲット設定
//! @param obj ターゲットオブジェクト
void Navmesh::SetTarget(const ObjectPtr& obj)
{
    // EnemyMoveと競合しないようにターゲットを解除
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        move->SetTarget(nullptr);
    }
    target_     = obj;
    has_target_ = false;
    RebuildPath();
}

//! @brief ターゲット設定
//! @param pos ターゲット位置
void Navmesh::SetTarget(const float3& pos)
{
    // EnemyMoveと競合しないようにターゲットを解除
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        move->SetTarget(nullptr);
    }
    target_.reset();
    has_target_ = true;
    target_pos_ = pos;
    RebuildPath();
}

//! @brief ポイントをスケーリング
//! @param p ポイント
//! @param s スケール
//! @return スケーリングされたポイント
float3 Navmesh::ScalePoint(const float3& p, float s) const
{
    return float3{p.x * s, p.y * s, p.z * s};
}

//! @brief ポイントを変換
//! @param m 行列
//! @param p ポイント
//! @return 変換されたポイント
float3 Navmesh::TransformPoint(const matrix& m, const float3& p) const
{
    float4 r = mul(m, float4(p.x, p.y, p.z, 1.0f));
    return float3{r.x, r.y, r.z};
}

//! @brief 右手系から左手系への変換行列を取得
//! @return 変換行列
matrix Navmesh::RHtoLHMatrix() const
{
    return matrix(MGetScale({1.0f, 1.0f, -1.0f}));
}

//! @brief 右手系から左手系へポイントを変換
//! @param p ポイント
//! @return 変換されたポイント
float3 Navmesh::RHtoLHPoint(const float3& p) const
{
    return float3{p.x, p.y, -p.z};
}

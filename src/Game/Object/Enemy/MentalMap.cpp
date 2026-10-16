//===========================================================================
//! @file MentalMap.cpp
//! @brief メンタルマップ
//===========================================================================
#include "MentalMap.h"
#include <System/Lib.h>

//! @brief 初期化
//! @return 初期化済み
bool MentalMap::Init()
{
    Super::Init();
    SetName(u8"メンタルマップ");

    // 読み込み
    MentalMapBuilder::LoadMap("data/Game/MentalMap/MentalMap.txt", data_);
    MentalMapBuilder::LoadAreaPointMap("data/Game/MentalMap/AreaPointMap.txt", area_point_data_, split_time_);

    // 必要なデータの初期化
    InitData();

    return true;
}

//! @brief 更新
void MentalMap::Update()
{
    Super::Update();
    if(!has_target_)
    {
        return;
    }

    // プレイヤーがどの領域にいるか探す
    for(int area_index = 0; area_index < mental_bounding_.size(); area_index++)
    {
        auto& bounding = mental_bounding_[area_index];
        if(player_pos_.x >= bounding.first.x && player_pos_.x <= bounding.second.x && player_pos_.z >= bounding.first.z && player_pos_.z <= bounding.second.z)
        {
            check_area_ = area_index;

            // どの点が近いか探す
            float min_distance = FLT_MAX;
            for(int point_index = 0; point_index < split_area_points_[area_index].size(); point_index++)
            {
                float3 point   = split_area_points_[area_index][point_index];
                float distance = length(point - player_pos_);
                if(distance < min_distance)
                {
                    min_distance   = distance;
                    closest_point_ = point_index;
                }
            }
            break;
        }
    }
}

//! @brief 遅延描画
void MentalMap::LateDraw()
{
    Super::LateDraw();

    if(!use_debug_)
    {
        return;
    }
    SetUseLighting(false);

    float3 start_pos = float3{data_.start_pos_.x, 200.0f, data_.start_pos_.z};
    float3 end_pos   = float3{data_.end_pos_.x, 200.0f, data_.end_pos_.z};
    DrawCube3D(cast(start_pos), cast(end_pos), GetColor(255, 0, 0), GetColor(255, 0, 0), false);

    if(use_map_debug_)
    {
        for(int area_index = 0; area_index < split_area_points_.size(); area_index++)
        {
            for(int point_index = 0; point_index < split_area_points_[area_index].size(); point_index++)
            {
                float3 point = split_area_points_[area_index][point_index];

                int color = split_area_points_color[area_index][point_index];
                DrawSphere3D(cast(point), 2.0f, 16, color, color, false);
            }
        }

        // バウンディングボックスの描画
        for(const auto& bounding : mental_bounding_)
        {
            float3 min_p = bounding.first;
            float3 max_p = bounding.second;
            min_p.y      = 200.0f;   // 見やすいため、高くする
            max_p.y      = 200.0f;
            DrawCube3D(cast(min_p), cast(max_p), GetColor(255, 255, 0), GetColor(255, 255, 0), false);
        }
    }
    if(check_area_ < 0 || check_area_ >= split_area_points_.size())
    {
        SetUseLighting(true);
        return;
    }

    MentalMap::MentalZoneInfo zone_info = GetMentalZoneInfo(check_area_, closest_point_);
    for(int point_index = 0; point_index < split_area_points_[check_area_].size(); point_index++)
    {
        float offset_y = 0.0f;
        if(use_map_debug_)
        {
            offset_y = 150.0f;
        }
        float3 point = split_area_points_[check_area_][point_index] + float3{0.0f, offset_y, 0.0f};
        if(point_index == closest_point_)
        {
            DrawSphere3D(cast(point), 3.0f, 16, GetColor(255, 0, 0), GetColor(255, 0, 0), false);

            // Zone0
            for(auto& p : zone_info.zone0_.points_)
            {
                float3 draw_pos = p + float3{0.0f, offset_y, 0.0f};
                DrawSphere3D(cast(draw_pos), 2.5f, 16, GetColor(255, 255, 255), GetColor(255, 255, 255), false);
            }

            // Zone1
            for(auto& zone : zone_info.zone1_)
            {
                for(auto& p : zone.points_)
                {
                    float3 draw_pos = p + float3{0.0f, offset_y, 0.0f};
                    DrawSphere3D(cast(draw_pos), 2.5f, 16, GetColor(0, 255, 255), GetColor(0, 255, 255), false);
                }
            }

            // Zone2
            for(const auto& zone : zone_info.zone2_)
            {
                for(const auto& p : zone.points_)
                {
                    float3 draw_pos = p + float3{0.0f, offset_y, 0.0f};
                    DrawSphere3D(cast(draw_pos), 2.5f, 16, GetColor(255, 255, 0), GetColor(255, 255, 0), false);
                }
            }
        }
    }

    SetUseLighting(true);
}

//! @brief GUI
void MentalMap::GUI()
{
    Super::GUI();

    ImGui::Begin(GetName().data());
    {
        ImGui::Separator();
        ImGui::Checkbox("Use Debug", &use_debug_);
        ImGui::Checkbox("Use Map Debug", &use_map_debug_);
        ImGui::DragInt("Split Time", &split_time_, 0.1f);
        if(ImGui::Button("Resplit Area"))
        {
            InitData();
        }

        int max_area = std::max(0, (int)split_area_points_.size() - 1);
        ImGui::DragInt("Check Area", &check_area_, 0.1f, 0, max_area);
        ImGui::Text(u8"領域の一つの大きさ: %f", split_area_range_);
    }
    ImGui::End();
}

//! @brief 終了
void MentalMap::Exit()
{
    Super::Exit();
    split_area_points_.clear();
    split_area_points_color.clear();
    mental_bounding_.clear();
    adjacent_areas_.clear();
}

//! @brief 領域の分割
void MentalMap::SplitArea()
{
    // クリア
    split_area_points_.clear();
    split_area_points_color.clear();
    bound_box.clear();
    // 四分木分割
    Quadtree(center_, map_size_, data_.mental_map_points_, 0, split_time_);
}

//! @brief データの初期化
void MentalMap::InitData()
{
    if(data_.mental_map_points_.empty())
    {
        return;
    }
    // データをクリア
    split_area_points_color.clear();
    mental_bounding_.clear();
    adjacent_areas_.clear();
    check_area_    = 0;
    closest_point_ = 0;

    // マップサイズと中心の計算
    map_size_.x = std::abs(data_.end_pos_.x - data_.start_pos_.x);
    map_size_.y = std::abs(data_.end_pos_.z - data_.start_pos_.z);
    center_.x   = (data_.end_pos_.x + data_.start_pos_.x) * 0.5f;
    center_.y   = (data_.end_pos_.z + data_.start_pos_.z) * 0.5f;

    // 領域の分割
    SplitArea();

    // 1領域の範囲計算
    float mid_size    = (map_size_.x + map_size_.y) * 0.5f;
    split_area_range_ = mid_size / (float)std::pow(2.0f, split_time_);

    float two_point_diff_x = split_area_points_[0][0].x - split_area_points_[0][1].x;
    two_point_diff_x       = abs(two_point_diff_x) * 0.25f;
    float two_point_diff_z = split_area_points_[0][0].z - split_area_points_[1][0].z;
    two_point_diff_z       = abs(two_point_diff_z) * 0.5f;

    mental_bounding_ = bound_box;

    // 隣接領域の計算
    adjacent_areas_.clear();
    adjacent_areas_.resize(mental_bounding_.size());

    for(int i = 0; i < mental_bounding_.size(); i++)
    {
        auto& bounding = mental_bounding_[i];
        for(int j = 0; j < mental_bounding_.size(); j++)
        {
            if(i == j)
            {
                continue;
            }
            auto& other_bounding = mental_bounding_[j];
            // 隣接判定
            bool is_adjacent = false;
            // 左右
            if((std::abs(bounding.first.x - other_bounding.second.x) < FLT_EPSILON &&
                !(bounding.second.z < other_bounding.first.z || bounding.first.z > other_bounding.second.z)) ||
               (std::abs(bounding.second.x - other_bounding.first.x) < FLT_EPSILON &&
                !(bounding.second.z < other_bounding.first.z || bounding.first.z > other_bounding.second.z)))
            {
                is_adjacent = true;
            }
            // 前後
            if((std::abs(bounding.first.z - other_bounding.second.z) < FLT_EPSILON &&
                !(bounding.second.x < other_bounding.first.x || bounding.first.x > other_bounding.second.x)) ||
               (std::abs(bounding.second.z - other_bounding.first.z) < FLT_EPSILON &&
                !(bounding.second.x < other_bounding.first.x || bounding.first.x > other_bounding.second.x)))
            {
                is_adjacent = true;
            }
            if(is_adjacent)
            {
                adjacent_areas_[i].push_back(j);
            }
        }
    }
}

//! @brief 四分木
//! @param center 現在の領域の中心
//! @param size 現在の領域のサイズ
//! @param points この領域に含まれる点
//! @param current_depth 現在の階層
//! @param max_depth 最大階層
void MentalMap::Quadtree(float2 center, float2 size, const std::vector<float3>& points, int current_depth, int max_depth)
{
    if(current_depth >= max_depth || points.empty())
    {
        if(!points.empty())
        {
            // 結果リストに追加
            split_area_points_.push_back(points);

            float3 box_min = {center.x - size.x * 0.5f, 0.0f, center.y - size.y * 0.5f};
            float3 box_max = {center.x + size.x * 0.5f, 0.0f, center.y + size.y * 0.5f};
            bound_box.emplace_back(std::make_pair(box_min, box_max));

            std::vector<int> colors;

            // 他の領域と区別するため、適当に色を決める
            int seed_x = int(center.x);
            int seed_y = int(center.y);
            int r      = (seed_x * 100 + seed_y * 50 + current_depth * 83) % 255;
            int g      = (seed_x * 200 + seed_y * 100 + current_depth * 24) % 255;
            int b      = (seed_x * 50 + seed_y * 200 + current_depth * 56) % 255;

            int color = GetColor(r, g, b);

            for(int i = 0; i < points.size(); i++)
            {
                colors.push_back(color);
            }

            split_area_points_color.push_back(colors);
        }
        return;
    }

    // 4つの領域用のリストを作成
    std::vector<float3> sub_area_points[4];

    // 現在の領域内の点を、中心座標を基準に4つに分ける
    // 左下→右下→左上→右上(3D回転(0.0,0.0,0.0)の場合)
    for(const auto& p : points)
    {
        bool is_left = (p.x < center.x);
        bool is_top  = (p.z < center.y);

        if(is_left && is_top)
        {
            sub_area_points[0].push_back(p);
        }
        else if(!is_left && is_top)
        {
            sub_area_points[1].push_back(p);
        }
        else if(is_left && !is_top)
        {
            sub_area_points[2].push_back(p);
        }
        else
        {
            sub_area_points[3].push_back(p);
        }
    }

    // 次のためにサイズを半分にする
    float2 half_size = {size.x / 2.0f, size.y / 2.0f};
    float2 quarter   = {size.x / 4.0f, size.y / 4.0f};

    Quadtree({center.x - quarter.x, center.y - quarter.y}, half_size, sub_area_points[0], current_depth + 1, max_depth);
    Quadtree({center.x + quarter.x, center.y - quarter.y}, half_size, sub_area_points[1], current_depth + 1, max_depth);
    Quadtree({center.x - quarter.x, center.y + quarter.y}, half_size, sub_area_points[2], current_depth + 1, max_depth);
    Quadtree({center.x + quarter.x, center.y + quarter.y}, half_size, sub_area_points[3], current_depth + 1, max_depth);
}

//! @brief 指定された点が属する領域を取得
//! @param position ターゲットの位置
std::vector<int> MentalMap::GetAdjacentAreas(const int& area_index)
{
    if(area_index < 0 || area_index >= adjacent_areas_.size())
    {
        return {};
    }
    return adjacent_areas_[area_index];
}

//! @brief 指定された領域に隣接する領域を取得
//! @param area_index ターゲット領域のインデックス
std::vector<int> MentalMap::GetAdjacentAreas(const float3& position)
{
    for(int i = 0; i < mental_bounding_.size(); i++)
    {
        auto& bounding = mental_bounding_[i];
        if(position.x >= bounding.first.x && position.x <= bounding.second.x && position.z >= bounding.first.z && position.z <= bounding.second.z)
        {
            return GetAdjacentAreas(i);
        }
    }
    return {};
}

//! @brief 指定された点から見える領域データを取得
//! @param area_index 現在の領域
//! @param point_index 現在の点
//! @return 可視領域データのリスト
std::vector<MentalMapBuilder::AreaPointData> MentalMap::GetVisibleData(int area_index, int point_index)
{
    std::vector<MentalMapBuilder::AreaPointData> result;
    for(const auto& data : area_point_data_)
    {
        if(data.area_index_ == area_index && data.point_index_ == point_index)
        {
            result.push_back(data);
        }
    }
    return result;
}

//! @brief 指定された領域のサブ領域の点を取得
//! @param area_index ターゲット領域
//! @param sub_bit サブ領域のビットフラグ(1:左下, 2:右下, 4:左上, 8:右上)
//! @return サブ領域に含まれる点のリスト
std::vector<float3> MentalMap::GetSubAreaPoints(int area_index, int sub_bit)
{
    if(area_index < 0 || area_index >= mental_bounding_.size())
    {
        return {};
    }
    // 中心とサイズを計算
    auto& bounding = mental_bounding_[area_index];
    float2 center;
    center.x = (bounding.first.x + bounding.second.x) * 0.5f;
    center.y = (bounding.first.z + bounding.second.z) * 0.5f;

    float2 size;
    size.x = bounding.second.x - bounding.first.x;
    size.y = bounding.second.z - bounding.first.z;

    // サブ領域の点を取得
    std::vector<std::vector<float3>> sub_split_points;
    MentalMapBuilder::Quadtree(center, size, split_area_points_[area_index], 0, 1, sub_split_points);

    // ビット検査
    std::vector<float3> result;
    for(int i = 0; i < sub_split_points.size(); i++)
    {
        if(sub_bit & (1 << i))
        {
            for(const auto& p : sub_split_points[i])
            {
                result.push_back(p);
            }
        }
    }

    return result;
}

//! @brief ゾーン情報を取得
//! @param area_index 現在の領域
//! @param point_index 現在の点
//! @return ゾーン情報
MentalMap::MentalZoneInfo MentalMap::GetMentalZoneInfo(int area_index, int point_index)
{
    MentalZoneInfo info;

    // Zone0: 現在の領域
    if(area_index >= 0 && area_index < split_area_points_.size())
    {
        info.zone0_.area_index_ = area_index;
        info.zone0_.sub_bit_    = 0;
        info.zone0_.points_     = split_area_points_[area_index];
    }

    // Zone1: 可視サブ隣接領域
    std::vector<MentalMapBuilder::AreaPointData> visible_data_list = GetVisibleData(area_index, point_index);
    for(const auto& data : visible_data_list)
    {
        ZoneData z_data;
        z_data.area_index_ = data.target_area_;
        z_data.sub_bit_    = data.target_sub_bit_;
        z_data.points_     = GetSubAreaPoints(data.target_area_, data.target_sub_bit_);
        info.zone1_.push_back(z_data);
    }

    // Zone2: 不可視サブ隣接領域
    std::vector<int> all_adjacent = GetAdjacentAreas(area_index);
    for(int adj_area : all_adjacent)
    {
        int visible_bit = 0;
        for(const auto& v_data : visible_data_list)
        {
            if(v_data.target_area_ == adj_area)
            {
                visible_bit |= v_data.target_sub_bit_;
            }
        }

        int invisible_bit = (~visible_bit) & 0x0F;

        if(invisible_bit != 0)
        {
            ZoneData z_data;
            z_data.area_index_ = adj_area;
            z_data.sub_bit_    = invisible_bit;
            z_data.points_     = GetSubAreaPoints(adj_area, invisible_bit);

            if(!z_data.points_.empty())
            {
                info.zone2_.push_back(z_data);
            }
        }
    }

    // Zoneの最大オブジェクト数設定
    // いい感じに何ポイントにつき1オブジェクト配置する
    const int POINT_PER_OBJECT = 20;
    info.zone0_.max_object_    = (int)(info.zone0_.points_.size() / POINT_PER_OBJECT);

    for(auto& z_data : info.zone1_)
    {
        z_data.max_object_ = (int)(z_data.points_.size() / POINT_PER_OBJECT);
    }

    // Zone0とZone1で配置されない場合は、全部Zone2に入れるため、多めに設定しておく
    for(auto& z_data : info.zone2_)
    {
        z_data.max_object_ = (int)z_data.points_.size();
    }

    return info;
}

//! @brief ゾーン情報を取得
//! @return ゾーン情報
MentalMap::MentalZoneInfo MentalMap::GetMentalZoneInfo()
{
    return GetMentalZoneInfo(check_area_, closest_point_);
}

//! @brief 敵のターゲット位置を取得
//! @param info ゾーン情報
//! @param my_pos 自分の位置
//! @param excluded_positions 除外する位置のリスト
//! @param prevent_radius プレイヤーとの距離がこの半径以内の点は除外
//! @param zone0_search_angle Zone0での検索角度
//! @param zone1_search_angle Zone1での検索角度
std::vector<float3> MentalMap::GetCandidatePoints(const std::vector<float3>& points,
                                                  const float3& my_pos,
                                                  const std::vector<float3>& excluded_positions,
                                                  const float prevent_radius,
                                                  const float search_angle,
                                                  const float prevent_rate,
                                                  const bool check_behind)
{
    std::vector<float3> candidate_points;
    const float EXCLUDE_RADIUS = 5.0f;   // 避けるための半径

    // 除外座標かどうかをチェックするラムダ
    auto is_excluded = [&](const float3& point) -> bool {
        for(const auto& excluded : excluded_positions)
        {
            float3 vec    = point - excluded;
            float dist_sq = dot(vec, vec);
            if(dist_sq < EXCLUDE_RADIUS * EXCLUDE_RADIUS)
            {
                return true;
            }
        }
        return false;
    };

    // 検索角度下げるラムダ
    auto adjust_search_angle = [&](const float3& point, const float angle, const float rate) -> float {
        float distance         = length(point - player_pos_);
        float prevent_distance = split_area_range_ * rate;
        float final_angle      = angle * (distance / prevent_distance);
        final_angle            = std::min(final_angle, angle);
        return final_angle;
    };

    for(const auto& point : points)
    {
        // 除外座標をスキップ
        if(is_excluded(point))
        {
            continue;
        }

        // 近ければ近いほど検索角度下げる
        float final_angle = adjust_search_angle(point, search_angle, prevent_rate);

        if(CheckPointEntryEnable(point, my_pos, prevent_radius, final_angle, check_behind))
        {
            float3 to_player = player_pos_ - my_pos;
            float3 to_point  = point - my_pos;
            if((float)dot(to_player, to_point) < 0.3f)
            {
                continue;
            }

            candidate_points.push_back(point);
            candidate_points_.push_back(point);
        }
    }
    return candidate_points;
}

//! @brief 敵のターゲット位置を取得
//! @param info ゾーン情報
//! @param my_pos 自分の位置
//! @param excluded_positions 除外する位置のリスト
//! @param prevent_radius プレイヤーとの距離がこの半径以内の点は除外
//! @param zone0_search_angle Zone0での検索角度
//! @param zone1_search_angle Zone1での検索角度
float3 MentalMap::GetEnemyTargetPos(MentalMap::MentalZoneInfo& info,
                                    const float3& my_pos,
                                    const std::vector<float3>& excluded_positions,
                                    const float prevent_radius,
                                    const float zone0_search_angle,
                                    const float zone1_search_angle)
{
    float3 target_pos = float3{0.0f, 0.0f, 0.0f};
    std::vector<float3> candidate_points;
    candidate_points_.clear();

    // 敵がどのゾーンに入っているか判定
    int my_zone_index = -1;
    // 敵がどのゾーンレベルにいるか判定（0,1,2）
    int my_zone_level = -1;
    for(int i = 0; i < mental_bounding_.size(); i++)
    {
        auto& bounding = mental_bounding_[i];
        if(my_pos.x >= bounding.first.x && my_pos.x <= bounding.second.x && my_pos.z >= bounding.first.z && my_pos.z <= bounding.second.z)
        {
            my_zone_index = i;
            break;
        }
    }

    if(my_zone_index == info.zone0_.area_index_)
    {
        // 基本はZone0を優先のため、0はいらないけど、一応設定しておく
        my_zone_level = 0;
    }
    else
    {
        for(const auto& zone1 : info.zone1_)
        {
            if(my_zone_index == zone1.area_index_)
            {
                my_zone_level = 1;
                break;
            }
        }
        if(my_zone_level < 0)
        {
            for(const auto& zone2 : info.zone2_)
            {
                if(my_zone_index == zone2.area_index_)
                {
                    my_zone_level = 2;
                    break;
                }
            }
        }
    }

    // Zone0を探す
    if(info.zone0_.object_ < info.zone0_.max_object_)
    {
        candidate_points = GetCandidatePoints(info.zone0_.points_, my_pos, excluded_positions, prevent_radius, zone0_search_angle, 0.5f);
    }
    if(!candidate_points.empty())
    {
        int point  = GetRand((int)(candidate_points.size()) - 1);
        target_pos = candidate_points[point];
        info.zone0_.object_++;
        return target_pos;
    }
    if(my_zone_level == 0 && info.zone0_.object_ < info.zone0_.max_object_)
    {
        info.zone0_.object_++;
        return my_pos;
    }

    // Zone1を探す
    std::vector<std::pair<int, std::vector<float3>>> candidates_zone1_behind;   // 後ろのZone1
    std::vector<std::pair<int, std::vector<float3>>> candidates_zone1_front;    // 前のZone1

    for(int i = 0; i < info.zone1_.size(); i++)
    {
        auto& zone1 = info.zone1_[i];
        if(zone1.object_ >= zone1.max_object_)
        {
            continue;
        }

        // 自分の後ろのZone1を優先的に探す
        if(my_zone_level == 0)
        {
            candidate_points = GetCandidatePoints(zone1.points_, my_pos, excluded_positions, prevent_radius, zone1_search_angle, 0.5f, true);
            if(!candidate_points.empty())
            {
                candidates_zone1_behind.emplace_back(i, candidate_points);
            }
        }

        // 前のZone1を探す
        candidate_points = GetCandidatePoints(zone1.points_, my_pos, excluded_positions, prevent_radius, zone1_search_angle, 0.5f, false);
        if(!candidate_points.empty())
        {
            candidates_zone1_front.emplace_back(i, candidate_points);
        }
    }

    // 後ろのZone1から優先的に選択（Zone0にいる場合）
    if(!candidates_zone1_behind.empty())
    {
        int zone_idx   = GetRand((int)(candidates_zone1_behind.size()) - 1);
        auto& selected = candidates_zone1_behind[zone_idx];
        int point_idx  = GetRand((int)(selected.second.size()) - 1);
        target_pos     = selected.second[point_idx];
        info.zone1_[selected.first].object_++;
        return target_pos;
    }

    // 全体のZone1から選択
    if(!candidates_zone1_front.empty())
    {
        int zone_idx   = GetRand((int)(candidates_zone1_front.size()) - 1);
        auto& selected = candidates_zone1_front[zone_idx];
        int point_idx  = GetRand((int)(selected.second.size()) - 1);
        target_pos     = selected.second[point_idx];
        info.zone1_[selected.first].object_++;
        return target_pos;
    }

    // プレイヤーとの距離が近い場合は、Zoneの点がないため、ここで距離を見る
    if(length(player_pos_ - my_pos) < (float1)prevent_radius * 2.0f)
    {
        return my_pos;
    }

    // Zone2を探す
    std::vector<std::pair<int, float3>> candidates_zone2_behind;   // 後ろのZone2
    std::vector<std::pair<int, float3>> candidates_zone2_all;      // 全体のZone2

    for(int i = 0; i < info.zone2_.size(); i++)
    {
        auto& zone2 = info.zone2_[i];
        if(zone2.object_ >= zone2.max_object_)
        {
            continue;
        }
        if(zone2.points_.empty())
        {
            continue;
        }

        // 優先的に後ろを探す
        if(my_zone_level > -1 && my_zone_level < 2)
        {
            candidate_points = GetCandidatePoints(zone2.points_, my_pos, excluded_positions, prevent_radius, 180.0f, 2.0f, true);
            if(!candidate_points.empty())
            {
                int point_idx = GetRand((int)(candidate_points.size()) - 1);
                candidates_zone2_behind.emplace_back(i, candidate_points[point_idx]);
            }
        }

        // 全体のZone2も探す
        float3 center = float3{0, 0, 0};
        for(const auto& p : zone2.points_)
        {
            center += p;
        }
        center /= (float)zone2.points_.size();
        candidates_zone2_all.emplace_back(i, center);
    }

    // 後ろのZone2から優先的に選択
    if(!candidates_zone2_behind.empty())
    {
        int zone_idx   = GetRand((int)(candidates_zone2_behind.size()) - 1);
        auto& selected = candidates_zone2_behind[zone_idx];
        target_pos     = selected.second;
        info.zone2_[selected.first].object_++;
        return target_pos;
    }

    // 一番近いZone2を探す
    if(!candidates_zone2_all.empty())
    {
        float min_dist    = FLT_MAX;
        int best_zone_idx = -1;
        for(int i = 0; i < candidates_zone2_all.size(); i++)
        {
            float dist = length(candidates_zone2_all[i].second - my_pos);
            if(dist < min_dist)
            {
                min_dist      = dist;
                best_zone_idx = i;
            }
        }
        if(best_zone_idx >= 0)
        {
            auto& zone2 = info.zone2_[candidates_zone2_all[best_zone_idx].first];
            if(!zone2.points_.empty())
            {
                int point  = GetRand((int)(zone2.points_.size()) - 1);
                target_pos = zone2.points_[point];
                zone2.object_++;
                return target_pos;
            }
        }
    }

    return my_pos;
}

//! @brief 敵の最も近いターゲット位置を取得
//! @param info ゾーン情報
//! @param my_pos 自分の位置
//! @param excluded_positions 除外する位置のリスト
//! @param prevent_radius プレイヤーとの距離がこの半径以内の点は除外
//! @param zone0_search_angle Zone0での検索角度
//! @param zone1_search_angle Zone1での検索角度
float3 MentalMap::GetEnemyNearestTargetPos(MentalMap::MentalZoneInfo& info,
                                           const float3& my_pos,
                                           const std::vector<float3>& excluded_positions,
                                           const float prevent_radius,
                                           const float zone0_search_angle,
                                           const float zone1_search_angle)
{
    float3 target_pos = float3{0.0f, 0.0f, 0.0f};
    std::vector<float3> candidate_points;
    candidate_points_.clear();

    // Zone0を探す
    if(info.zone0_.object_ < info.zone0_.max_object_)
    {
        candidate_points = GetCandidatePoints(info.zone0_.points_, my_pos, excluded_positions, prevent_radius, zone0_search_angle, 1.0f);
    }
    // Zone0で見つかる場合、一番近い点を探す
    if(!candidate_points.empty())
    {
        float min_dist         = FLT_MAX;
        int target_point_index = -1;
        for(int i = 0; i < candidate_points.size(); i++)
        {
            float dist = length(candidate_points[i] - player_pos_);
            if(dist < min_dist)
            {
                min_dist           = dist;
                target_point_index = i;
            }
        }

        if(target_point_index != -1)
        {
            target_pos = candidate_points[target_point_index];
            info.zone0_.object_++;
            return target_pos;
        }
    }

    // Zone1を探す
    float min_zone1_dist = FLT_MAX;
    int best_zone_idx    = -1;
    float3 best_pos      = float3{0.0f, 0.0f, 0.0f};

    for(int i = 0; i < info.zone1_.size(); i++)
    {
        auto& zone1 = info.zone1_[i];
        if(zone1.object_ >= zone1.max_object_)
        {
            continue;
        }

        candidate_points = GetCandidatePoints(zone1.points_, my_pos, excluded_positions, prevent_radius, zone1_search_angle, 2.0f);

        if(!candidate_points.empty())
        {
            for(const auto& p : candidate_points)
            {
                float dist = length(p - player_pos_);
                if(dist < min_zone1_dist)
                {
                    min_zone1_dist = dist;
                    best_pos       = p;
                    best_zone_idx  = i;
                }
            }
        }
    }

    // Zone1で見つかる場合
    if(best_zone_idx != -1)
    {
        target_pos = best_pos;
        info.zone1_[best_zone_idx].object_++;
        return target_pos;
    }

    info.zone0_.object_++;
    return my_pos;
}

//! @brief 点の位置が敵が入れるかどうかをチェック
//! @param point_pos チェックする点の位置
//! @param enemy_pos 敵の位置
//! @param prevent_radius プレイヤーとの距離がこの半径以内の点は除外
//! @param search_angle 検索角度
//! @param check_behind 敵の背後もチェックするかどうか
bool MentalMap::CheckPointEntryEnable(const float3& point_pos, const float3& enemy_pos, const float prevent_radius, const float search_angle, bool check_behind)
{
    // プレイヤーに近すぎる点は不可
    float distance = length(point_pos - player_pos_);
    if(distance < prevent_radius)
    {
        return false;
    }
    float3 player_vec      = player_pos_ - enemy_pos;
    float3 enemy_point_vec = point_pos - enemy_pos;

    if(check_behind)
    {
        // 点が敵の後ろにある、後ろなら可
        if(dot(player_vec, enemy_point_vec) > (float1)0.0f)
        {
            return false;
        }

        // 後ろの45度以内に点があるかどうか
        float3 enemy_vec        = normalize(player_pos_ - enemy_pos);
        float3 player_point_vec = normalize(point_pos - enemy_pos);
        // 敵の後ろ方向ベクトル
        float3 enemy_back_vec = -enemy_vec;
        if(IsInDotAngle(enemy_back_vec, player_point_vec, 45.0f))
        {
            return true;
        }

        return false;
    }
    else
    {
        // 点が敵の後ろにあるかどうか、後ろの点は不可
        if(dot(player_vec, enemy_point_vec) < (float1)0.0f)
        {
            return false;
        }

        // プレイヤの後ろの点は不可
        if(dot(player_vec, point_pos - player_pos_) > (float1)0.0f)
        {
            return false;
        }

        // 指定角度内に点があるかどうか
        float3 enemy_vec        = normalize(player_pos_ - enemy_pos);
        float3 player_point_vec = normalize(point_pos - enemy_pos);
        if(IsInDotAngle(enemy_vec, player_point_vec, search_angle))
        {
            return true;
        }

        return false;
    }
}

//! @brief ゾーン内のオブジェクト数をクリア
//! @param info クリアしたいゾーン情報
void MentalMap::ClearZoneObject(MentalMap::MentalZoneInfo& info)
{
    info.zone0_.object_ = 0;
    for(auto& z_data : info.zone1_)
    {
        z_data.object_ = 0;
    }
    for(auto& z_data : info.zone2_)
    {
        z_data.object_ = 0;
    }
}

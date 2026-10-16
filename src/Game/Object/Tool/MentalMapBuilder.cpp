//===========================================================================
//! @file MentalMapBuilder.cpp
//! @brief メンタルマップビルダー
//===========================================================================
#include "MentalMapBuilder.h"
#include <iomanip>
#include <System/Debug/DebugCamera.h>
#include <Game/Object/Tool/MentalPosBall.h>
#include <Game/Object/Tool/AddData.h>
#include <Game/Component/Navmesh.h>
#include <Game/Object/Enemy/MentalMap.h>
#include <System/Component/ComponentModel.h>
#include <Game/Object/Stage.h>

//! @brief 初期化
//! @return 初期化済み
bool MentalMapBuilder::Init()
{
    Super::Init();
    SetName(u8"メンタルマップビルダー");
    data_.start_pos_ = float3{-100.0f, 250.0f, -100.0f};
    data_.end_pos_   = float3{100.0f, 250.0f, 100.0f};
    return true;
}

//! @brief 更新
void MentalMapBuilder::Update()
{
    Super::Update();
    if(is_generating_)
    {
        GenerateMap();
    }
    data_.start_pos_.y = data_.end_pos_.y;
}

//! @brief 遅延描画
void MentalMapBuilder::LateDraw()
{
    Super::LateDraw();
    if(!use_debug_)
    {
        return;
    }
    SetUseLighting(false);
    DrawCube3D(cast(data_.start_pos_), cast(data_.end_pos_), GetColor(255, 0, 0), GetColor(255, 0, 0), false);
    DrawSphere3D(cast(data_.start_pos_), 2.0f, 16, GetColor(0, 255, 0), GetColor(0, 255, 0), true);
    DrawSphere3D(cast(data_.end_pos_), 2.0f, 16, GetColor(0, 0, 255), GetColor(0, 0, 255), true);

    for(auto& point : data_.mental_map_points_)
    {
        DrawSphere3D(cast(point), 1.0f, 16, GetColor(255, 255, 255), GetColor(255, 255, 255), true);
    }

    if(auto mental_map = Scene::Object::Get<MentalMap>())
    {
        const auto& split_area_points = mental_map->GetSplitAreaPoints();
        if(!split_area_points.empty())
        {
            if(check_area_ >= 0 && check_area_ < split_area_points.size())
            {
                auto& points = split_area_points[check_area_];
                for(auto& p : points)
                {
                    DrawSphere3D(cast(p), 1.5f, 16, GetColor(0, 255, 0), GetColor(0, 255, 0), true);
                }
            }
        }
    }

    SetUseLighting(true);
}

//! @brief GUI
void MentalMapBuilder::GUI()
{
    Super::GUI();

    ImGui::Begin(GetName().data());
    {
        ImGui::Separator();

        ImGui::InputText("Save Path", &file_path_);
        ImGui::Checkbox("Use Debug Draw", &use_debug_);

        ImGui::DragFloat3("Generate Pos1", (float*)&data_.start_pos_);
        ImGui::DragFloat3("Generate Pos2", (float*)&data_.end_pos_);
        ImGui::DragInt("Spawn Num", &num_, 0.1f, 0, 10000);
        if(ImGui::Button("Generate Map"))
        {
            auto ball_array = Scene::Object::GetArray<MentalPosBall>();
            for(auto& ball : ball_array)
            {
                Scene::Object::Release(ball);
            }
            data_.mental_map_points_.clear();
            is_generating_    = true;
            now_generate_row_ = 0;
            now_generate_col_ = 0;
        }
        if(ImGui::Button("Apply Navmesh"))
        {
            ApplyNavmesh();
        }

        if(ImGui::Button(("Save Map")))
        {
            if(!file_path_.empty())
            {
                SaveMap();
            }
        }
        if(ImGui::Button(("Load Map")))
        {
            if(!file_path_.empty())
            {
                LoadMap(file_path_, data_);
            }
        }
        ImGui::Text(debug_text_.c_str());
        ImGui::Separator();

        ImGui::Text(u8"追加オプション(MentalMap存在する必要ある)");
        ImGui::InputText("Save Path##AreaPointMap", &visibility_file_path_);
        if(ImGui::Button("Build AreaPointMap"))
        {
            BuildAreaPointMap();
        }

        if(ImGui::Button("Save AreaPointMap"))
        {
            SaveAreaPointMap();
        }
    }
    ImGui::End();
}

//! @brief 終了
void MentalMapBuilder::Exit()
{
    Super::Exit();
    auto ball_array = Scene::Object::GetArray<MentalPosBall>();
    for(auto& ball : ball_array)
    {
        Scene::Object::Release(ball);
    }
}

//! @brief マップ生成
void MentalMapBuilder::GenerateMap()
{
    // 範囲内にずらして生成
    float3 range = data_.end_pos_ - data_.start_pos_;

    const int MAX_PER_FRAME = 50;

    int count = 0;
    for(; now_generate_row_ < num_; now_generate_row_++)
    {
        for(; now_generate_col_ < num_; now_generate_col_++)
        {
            auto obj = Scene::Object::Create<MentalPosBall>();
            if(now_generate_col_ % 2 == 0)
            {
                obj->SetTranslate(float3{data_.start_pos_.x + (range.x / num_) * 0.25f + now_generate_row_ * range.x / num_,
                                         data_.start_pos_.y,
                                         data_.start_pos_.z + (range.z / num_) * 0.5f + now_generate_col_ * range.z / num_});
            }
            else
            {
                obj->SetTranslate(float3{data_.start_pos_.x + (range.x / num_) * 0.75f + now_generate_row_ * range.x / num_,
                                         data_.start_pos_.y,
                                         data_.start_pos_.z + (range.z / num_) * 0.5f + now_generate_col_ * range.z / num_});
            }

            count++;
            if(count >= MAX_PER_FRAME)
            {
                now_generate_col_++;
                if(now_generate_col_ >= num_)
                {
                    now_generate_row_++;
                    now_generate_col_ = 0;
                }
                return;
            }
        }
        now_generate_col_ = 0;
    }

    if(now_generate_row_ >= num_)
    {
        is_generating_    = false;
        now_generate_row_ = num_;
        debug_text_       = u8"ビルド完成";
    }
}

//! @brief ナビメッシュ適用
void MentalMapBuilder::ApplyNavmesh()
{
    auto navmesh = GetComponent<Navmesh>();
    if(!navmesh)
    {
        debug_text_ = u8"ナビメッシュがない";
        return;
    }
    // 生成したボールの位置をナビメッシュに反映
    // ナビメッシュにその位置がなかったら消す
    std::vector<float3> valid_points;
    for(auto& point : data_.mental_map_points_)
    {
        if(navmesh->IsInNavmesh(point, navmesh_target_))
        {
            valid_points.push_back(point);
        }
    }
    data_.mental_map_points_ = valid_points;
    debug_text_              = u8"適用完了";
}

//! @brief マップ保存
void MentalMapBuilder::SaveMap()
{
    auto add_data = Scene::Object::Get<AddData>();
    add_data->SetPath(file_path_);
    std::ofstream file(file_path_, std::ios::trunc);
    if(!file.is_open())
    {
        debug_text_ = u8"セーブ失敗";
        return;
    }
    file << std::fixed << std::setprecision(2);
    SaveData data;
    data.start_pos_         = data_.start_pos_;
    data.end_pos_           = data_.end_pos_;
    data.mental_map_points_ = data_.mental_map_points_;

    file << data.start_pos_.x << "," << data.start_pos_.y << "," << data.start_pos_.z << "\n";
    file << data.end_pos_.x << "," << data.end_pos_.y << "," << data.end_pos_.z << "\n";

    for(auto& point : data.mental_map_points_)
    {
        file << point.x << "," << point.y << "," << point.z << "\n";
    }

    file.close();

    debug_text_ = u8"セーブ成功";
}

//! @brief マップ読み込み
void MentalMapBuilder::LoadMap(std::string file_path, SaveData& data)
{
    std::ifstream file(file_path);
    if(!file.is_open())
    {
        return;
    }
    data.mental_map_points_.clear();
    std::string line;

    // マップサイズの読み込み
    std::getline(file, line);
    {
        auto values       = HelperLib::String::Split(line, ',');
        data.start_pos_.x = std::stof(values[0]);
        data.start_pos_.y = std::stof(values[1]);
        data.start_pos_.z = std::stof(values[2]);
    }
    std::getline(file, line);
    {
        auto values     = HelperLib::String::Split(line, ',');
        data.end_pos_.x = std::stof(values[0]);
        data.end_pos_.y = std::stof(values[1]);
        data.end_pos_.z = std::stof(values[2]);
    }

    while(std::getline(file, line))
    {
        if(line.empty())
        {
            continue;
        }
        // カンマで分割
        auto values = HelperLib::String::Split(line, ',');
        // データを追加
        float3 pos;
        pos.x = std::stof(values[0]);
        pos.y = std::stof(values[1]);
        pos.z = std::stof(values[2]);

        data.mental_map_points_.emplace_back(pos);
    }
    file.close();
}

//! @brief 隣接領域の可視性保存
void MentalMapBuilder::SaveAreaPointMap()
{
    if(built_split_time_ < 0)
    {
        debug_text_ = u8"先に可視性データをビルドしてください";
        return;
    }
    // ファイル保存
    std::ofstream file(visibility_file_path_);
    if(file.is_open())
    {
        file << "split_time," << built_split_time_ << "\n";
        for(const auto& data : area_point_data_)
        {
            file << data.area_index_ << "," << data.point_index_ << "," << data.target_area_ << "," << data.target_sub_bit_ << "\n";
        }
        file.close();
        debug_text_ = u8"セーブ成功";
    }
    else
    {
        debug_text_ = u8"セーブ失敗";
    }
}

//! @brief 隣接領域の可視性読み込み
void MentalMapBuilder::LoadAreaPointMap(std::string file_path, std::vector<AreaPointData>& data, int& split_time)
{
    data.clear();
    std::ifstream file(file_path);
    if(!file.is_open())
    {
        return;
    }
    std::string line;
    while(std::getline(file, line))
    {
        if(line.empty())
        {
            continue;
        }
        // カンマで分割
        auto values = HelperLib::String::Split(line, ',');
        if(values[0] == "split_time")
        {
            split_time = std::stoi(values[1]);
            continue;
        }
        // データを追加
        AreaPointData ap_data;
        ap_data.area_index_     = std::stoi(values[0]);
        ap_data.point_index_    = std::stoi(values[1]);
        ap_data.target_area_    = std::stoi(values[2]);
        ap_data.target_sub_bit_ = std::stoi(values[3]);
        data.emplace_back(ap_data);
    }
    file.close();
}

//! @brief 隣接領域の可視性ビルド
void MentalMapBuilder::BuildAreaPointMap()
{
    auto mental_map = Scene::Object::Get<MentalMap>();
    if(!mental_map)
    {
        debug_text_ = u8"MentalMapが見つかりません";
        return;
    }

    int init_split_time = mental_map->GetSplitTime();
    mental_map->SetSplitTime(init_split_time);

    auto& mental_bounding   = mental_map->GetMentalBounding();
    auto& split_area_points = mental_map->GetSplitAreaPoints();

    // ステージモデルの取得
    int stage_model_handle = -1;
    if(auto stage = Scene::Object::Get<Stage>())
    {
        if(auto model = stage->GetComponent<ComponentModel>())
        {
            stage_model_handle = model->GetModel();
        }
    }

    area_point_data_.clear();

    // 全領域
    for(int area_index = 0; area_index < split_area_points.size(); area_index++)
    {
        const auto& my_points           = split_area_points[area_index];
        std::vector<int> adjacent_areas = mental_map->GetAdjacentAreas(area_index);

        for(int point_index = 0; point_index < my_points.size(); point_index++)
        {
            float3 my_point = my_points[point_index];

            // 隣接領域ごとにチェック
            for(int adj_area : adjacent_areas)
            {
                if(adj_area < 0 || adj_area >= mental_bounding.size())
                {
                    continue;
                }

                auto& other_bounding = mental_bounding[adj_area];

                // 隣接領域の中心とサイズ
                float2 other_center;
                other_center.x = (other_bounding.first.x + other_bounding.second.x) * 0.5f;
                other_center.y = (other_bounding.first.z + other_bounding.second.z) * 0.5f;

                float2 other_size;
                other_size.x = other_bounding.second.x - other_bounding.first.x;
                other_size.y = other_bounding.second.z - other_bounding.first.z;

                // 隣接領域をさらに分割
                std::vector<std::vector<float3>> sub_split_points;
                if(adj_area < split_area_points.size())
                {
                    Quadtree(other_center, other_size, split_area_points[adj_area], 0, 1, sub_split_points);
                }

                AreaPointData data;
                data.target_sub_bit_ = 0;

                // 分割されたサブ領域をチェック
                for(int sub_area = 0; sub_area < sub_split_points.size(); sub_area++)
                {
                    auto& sub_points = sub_split_points[sub_area];
                    if(sub_points.empty())
                    {
                        continue;
                    }

                    // サブ領域
                    float3 sub_min = sub_points[0];
                    float3 sub_max = sub_points[0];
                    for(auto& p : sub_points)
                    {
                        sub_min.x = std::min(sub_min.x, p.x);
                        sub_min.y = std::min(sub_min.y, p.y);
                        sub_min.z = std::min(sub_min.z, p.z);
                        sub_max.x = std::max(sub_max.x, p.x);
                        sub_max.y = std::max(sub_max.y, p.y);
                        sub_max.z = std::max(sub_max.z, p.z);
                    }
                    float3 sub_center = (sub_min + sub_max) * 0.5f;
                    sub_center.y      = sub_max.y;

                    // 地面から少し浮かす
                    my_point.y   += 3.0f;
                    sub_center.y += 1.0f;

                    // レイキャスト
                    MV1_COLL_RESULT_POLY hit_poly = MV1CollCheck_Line(stage_model_handle, -1, cast(my_point), cast(sub_center));

                    if(!hit_poly.HitFlag)
                    {
                        data.area_index_  = area_index;
                        data.point_index_ = point_index;   // エリア内
                        data.target_area_ = adj_area;
                        // ビットを追加
                        data.target_sub_bit_ |= (1 << sub_area);
                    }
                }
                // 最後にまとめて追加
                if(data.target_sub_bit_ != 0)
                {
                    area_point_data_.emplace_back(data);
                }
            }
        }
    }
    built_split_time_ = init_split_time;
    debug_text_       = u8"データビルド完了";
}

//! @brief 四分木
//! @param center 現在の領域の中心
//! @param size 現在の領域のサイズ
//! @param points この領域に含まれる点
//! @param current_depth 現在の階層
//! @param max_depth 最大階層
//! @param out_points 出力先
void MentalMapBuilder::Quadtree(
    float2 center, float2 size, const std::vector<float3>& points, int current_depth, int max_depth, std::vector<std::vector<float3>>& out_points)
{
    if(current_depth >= max_depth || points.empty())
    {
        if(!points.empty())
        {
            out_points.push_back(points);
        }
        return;
    }

    // 4つの領域用のリストを作成
    std::vector<float3> sub_area_points[4];

    // 現在の領域内の点を、中心座標を基準に4つに振り分ける
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

    // 次の階層のためにサイズを半分にする
    float2 half_size = {size.x / 2.0f, size.y / 2.0f};
    float2 quarter   = {size.x / 4.0f, size.y / 4.0f};   // 中心点のオフセット量

    Quadtree({center.x - quarter.x, center.y - quarter.y}, half_size, sub_area_points[0], current_depth + 1, max_depth, out_points);
    Quadtree({center.x + quarter.x, center.y - quarter.y}, half_size, sub_area_points[1], current_depth + 1, max_depth, out_points);
    Quadtree({center.x - quarter.x, center.y + quarter.y}, half_size, sub_area_points[2], current_depth + 1, max_depth, out_points);
    Quadtree({center.x + quarter.x, center.y + quarter.y}, half_size, sub_area_points[3], current_depth + 1, max_depth, out_points);
}

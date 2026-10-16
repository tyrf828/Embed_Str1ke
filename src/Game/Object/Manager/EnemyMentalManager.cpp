//===========================================================================
//! @file EnemyMentalManager.cpp
//! @brief メンタルマップマネージャー
//===========================================================================
#include "EnemyMentalManager.h"
#include <Game/Object/Player/Player.h>
#include <Game/Object/Enemy/EnemyBase.h>
#include <Game/Component/Enemy/EnemyActionBase.h>
#include <Game/Component/Enemy/ActionBase/ActionThreatBase.h>
#include <Game/Component/Navmesh.h>
#include <Game/Component/Enemy/ActionBase/ActionAttackBase.h>
#include <Game/Component/Enemy/ActionBase/ActionFindPlayerBase.h>

//! @brief 初期化
bool EnemyMentalManager::Init()
{
    Super::Init();
    SetName("EnemyMentalManager");

    return true;
}

//! @brief 更新
void EnemyMentalManager::Update()
{
    Super::Update();

    auto mental_map = Scene::Object::Get<MentalMap>();
    if(!mental_map)
    {
        return;
    }

    auto player = Scene::Object::Get<Player>();
    if(!player)
    {
        return;
    }
    // プレイヤー位置更新
    mental_map->SetPlayerPos(player->GetTranslate());

    int current_area = mental_map->GetCurrentArea();
    // ゾーン変更の判定
    is_zone_changed_ = false;
    if(last_area != current_area)
    {
        last_area        = current_area;
        is_zone_changed_ = true;
        // ゾーンが変わったら即更新だと、Zone0の端っこが判定されてないから、少し遅れて更新する
        update_timer_ = UPDATE_INTERVAL_ * 0.5f;
    }

    update_timer_ += GetDeltaTime();
    if(update_timer_ < UPDATE_INTERVAL_)
    {
        return;
    }
    update_timer_ = 0.0f;

    // 現在のゾーン情報を取得してローカルに保持
    zone_info_ = mental_map->GetMentalZoneInfo();

    if(enemies_.empty())
    {
        return;
    }

    // 距離を近い順にソート
    std::vector<std::pair<float, EnemyBasePtr>> enemy_distances;
    enemy_distances.reserve(enemies_.size());
    float3 player_pos = player->GetTranslate();

    for(auto& enemy : enemies_)
    {
        float dist = FLT_MAX;

        float3 check_pos = enemy->GetTranslate();
        if(auto nav = enemy->GetComponent<Navmesh>())
        {
            if(nav->IsUseNavmesh())
            {
                check_pos = nav->GetTargetPos();
            }
            dist = nav->GetNavmeshDistance(check_pos, player_pos);
        }
        else
        {
            dist = length(check_pos - player_pos);
        }
        enemy_distances.push_back({dist, enemy});
    }

    std::sort(enemy_distances.begin(), enemy_distances.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

    for(int i = 0; i < enemies_.size(); i++)
    {
        enemies_[i] = enemy_distances[i].second;
    }

    record_target_pos.clear();

    // 既に割り当てられた座標を追跡
    std::vector<float3> assigned_positions;

    ClosestEnemyUpdate(mental_map, player, assigned_positions);

    // 各敵にゾーン情報を更新
    for(auto& enemy : enemies_)
    {
        auto action = enemy->GetComponent<EnemyActionBase>();
        if(!action)
        {
            continue;
        }

        // 既にターゲットが設定されている場合はスキップ
        if(record_target_pos.find(enemy) != record_target_pos.end())
        {
            continue;
        }

        float3 prefer_pos        = GetPreferPosition(enemy->GetTranslate(), assigned_positions);
        record_target_pos[enemy] = prefer_pos;
        assigned_positions.push_back(prefer_pos);

        // もし敵が既に所属すべきゾーンにいるなら、スキップ
        if(IsInValidEnemyZone(enemy->GetTranslate(), prefer_pos))
        {
            continue;
        }
        auto current_action = action->GetCurrentAction();

        if(!dynamic_cast<ActionFindPlayerBase*>(current_action.get()) && !dynamic_cast<ActionThreatBase*>(current_action.get()))
        {
            continue;
        }

        // 距離が近すぎる場合は無視
        if(auto threat_action = dynamic_cast<ActionThreatBase*>(current_action.get()))
        {
            float distance = length(enemy->GetTranslate() - player->GetTranslate());
            if(distance < threat_action->GetThreatMinDistance())
            {
                continue;
            }
        }

        // ゾーンが変わってないなら、既に移動している敵はスキップ
        if(!is_zone_changed_)
        {
            int area = GetAreaIndex(enemy->GetTranslate());
            int zone = GetZoneLevel(area, enemy->GetTranslate());
            if(auto nav = enemy->GetComponent<Navmesh>())
            {
                if((zone >= 0 && zone < 2) && nav->IsUseNavmesh())
                {
                    continue;
                }
            }
        }

        action->SetFindPlayer();
    }
}

//! @brief 敵が向かうべきターゲット座標を取得する
//! @param enemy_pos 敵の現在位置
//! @param excluded_positions 除外する座標
//! @return ターゲット座標
float3 EnemyMentalManager::GetPreferPosition(const float3& enemy_pos, const std::vector<float3>& excluded_positions)
{
    auto mental_map = Scene::Object::Get<MentalMap>();
    if(!mental_map)
    {
        return float3(0, 0, 0);
    }

    return mental_map->GetEnemyTargetPos(zone_info_, enemy_pos, excluded_positions);
}

//! @brief 自分は有効なゾーンに含まれるか判定
//! @param enemy_pos 敵の位置
//! @param target_pos ターゲットにしたい位置
bool EnemyMentalManager::IsInValidEnemyZone(const float3& enemy_pos, const float3& target_pos)
{
    // 敵の現在位置がどのエリアに所属しているか判定
    int enemy_area_index = GetAreaIndex(enemy_pos);
    if(enemy_area_index < 0)
    {
        return false;
    }

    // ターゲット位置がどのエリアに所属しているか判定
    int target_area_index = GetAreaIndex(target_pos);
    if(target_area_index < 0)
    {
        return false;
    }

    int enemy_zone_level  = GetZoneLevel(enemy_area_index, enemy_pos);
    int target_zone_level = GetZoneLevel(target_area_index, target_pos);

    // 両方のゾーンレベルが有効で、かつ同じゾーンレベルであれば有効
    if(enemy_zone_level >= 0 && target_zone_level >= 0 && enemy_zone_level == target_zone_level)
    {
        return true;
    }

    return false;
}

//! @brief 敵を登録
//! @param enemy 登録する敵
void EnemyMentalManager::RegisterEnemyList(EnemyBasePtr enemy)
{
    if(std::find(enemies_.begin(), enemies_.end(), enemy) != enemies_.end())
    {
        return;
    }
    enemies_.push_back(enemy);
}

//! @brief 敵のターゲット位置を記録
//! @param enemy ターゲット位置を記録する敵
float3 EnemyMentalManager::GetRecordTargetPos(EnemyBasePtr enemy)
{
    auto record = record_target_pos.find(enemy);
    if(record != record_target_pos.end())
    {
        return record->second;
    }
    // 見つからなかったここで計算
    std::vector<float3> existing_positions;
    for(const auto& pair : record_target_pos)
    {
        existing_positions.push_back(pair.second);
    }
    float3 prefer_pos = GetPreferPosition(enemy->GetTranslate(), existing_positions);

    record_target_pos[enemy] = prefer_pos;

    return prefer_pos;
}

//! @brief GUI
void EnemyMentalManager::GUI()
{
    Super::GUI();
    ImGui::Begin(GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("Current Player Area: %d", zone_info_.zone0_.area_index_);
        // 敵が今どのゾーンにいるか
        for(const auto& pair : record_target_pos)
        {
            auto enemy        = pair.first;
            float3 target_pos = pair.second;
            int area_index    = GetAreaIndex(target_pos);
            ImGui::Text("Enemy: %s Target Area: %d", enemy->GetName().data(), area_index);
        }
    }
    ImGui::End();
}

//! @brief 敵の位置からエリアインデックスを取得
int EnemyMentalManager::GetAreaIndex(const float3& pos)
{
    auto mental_map = Scene::Object::Get<MentalMap>();
    if(!mental_map)
    {
        return -1;
    }

    const auto& bounding_boxes = mental_map->GetMentalBounding();
    for(int i = 0; i < bounding_boxes.size(); i++)
    {
        const auto& box = bounding_boxes[i];
        if(pos.x >= box.first.x && pos.x <= box.second.x && pos.z >= box.first.z && pos.z <= box.second.z)
        {
            return i;
        }
    }
    return -1;
}

//! @brief 指定された位置のゾーンレベルを取得
int EnemyMentalManager::GetZoneLevel(int area_index, const float3& pos)
{
    auto mental_map = Scene::Object::Get<MentalMap>();
    if(!mental_map)
    {
        return -1;
    }

    // Zone0に含まれるか確認
    if(zone_info_.zone0_.area_index_ == area_index)
    {
        return 0;
    }

    const auto& bounding_boxes = mental_map->GetMentalBounding();
    if(area_index < 0 || area_index >= bounding_boxes.size())
    {
        return -1;
    }

    // サブビットを計算
    const auto& box = bounding_boxes[area_index];
    float center_x  = (box.first.x + box.second.x) * 0.5f;
    float center_z  = (box.first.z + box.second.z) * 0.5f;

    int sub_index = 0;
    bool is_left  = (pos.x < center_x);
    bool is_top   = (pos.z < center_z);

    if(is_left && is_top)
    {
        sub_index = 0;   // 左下
    }
    else if(!is_left && is_top)
    {
        sub_index = 1;   // 右下
    }
    else if(is_left && !is_top)
    {
        sub_index = 2;   // 左上
    }
    else
    {
        sub_index = 3;   // 右上
    }

    int sub_bit = (1 << sub_index);

    // Zone1に含まれるか確認
    for(const auto& zone1 : zone_info_.zone1_)
    {
        if(zone1.area_index_ == area_index && (zone1.sub_bit_ & sub_bit))
        {
            return 1;
        }
    }

    // Zone2に含まれるか確認
    for(const auto& zone2 : zone_info_.zone2_)
    {
        if(zone2.area_index_ == area_index && (zone2.sub_bit_ & sub_bit))
        {
            return 2;
        }
    }

    // どのゾーンにも含まれない
    return -1;
}

//! @brief 最も近い敵の位置を更新
//! @param mental_map メンタルマップ
//! @param player プレイヤー
//! @param assigned_positions 既に割り当てられた位置
void EnemyMentalManager::ClosestEnemyUpdate(MentalMapPtr mental_map, PlayerPtr player, std::vector<float3>& assigned_positions)
{
    if(!enemies_.empty())
    {
        auto& nearest_enemy = enemies_[0];
        auto action         = nearest_enemy->GetComponent<EnemyActionBase>();
        if(action)
        {
            auto current_action = action->GetCurrentAction();

            // 威嚇状態かどうか
            if(auto threat_action = dynamic_cast<ActionThreatBase*>(current_action.get()))
            {
                float3 enemy_pos = nearest_enemy->GetTranslate();
                float distance   = length(enemy_pos - player->GetTranslate());
                auto temp_info   = zone_info_;

                if(distance > threat_action->GetThreatMaxDistance() - threat_action->GetThreatMinDistance())
                {
                    record_target_pos[nearest_enemy] = mental_map->GetEnemyNearestTargetPos(zone_info_, nearest_enemy->GetTranslate(), assigned_positions);
                    assigned_positions.push_back(record_target_pos[nearest_enemy]);
                }
                else
                {
                    // 威嚇範囲内なら現在位置をターゲットに設定
                    record_target_pos[nearest_enemy] = nearest_enemy->GetTranslate();
                    assigned_positions.push_back(record_target_pos[nearest_enemy]);
                    zone_info_.zone0_.object_++;
                }
            }
            // プレイヤーを探す状態かどうか
            if(dynamic_cast<ActionFindPlayerBase*>(current_action.get()))
            {
                float3 target_pos                = mental_map->GetEnemyNearestTargetPos(zone_info_, nearest_enemy->GetTranslate(), assigned_positions);
                record_target_pos[nearest_enemy] = target_pos;
                assigned_positions.push_back(record_target_pos[nearest_enemy]);
            }
            if(dynamic_cast<ActionFindPlayerBase*>(current_action.get()) || dynamic_cast<ActionThreatBase*>(current_action.get()))
            {
                action->SetFindPlayer();
            }
        }
    }
}

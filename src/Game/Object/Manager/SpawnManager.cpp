//===========================================================================
//! @file SpawnManager.cpp
//! @brief スポーンマネージャー
//===========================================================================
#include <Game/Object/Manager/SpawnManager.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Object/Player/Player.h>
#include <Game/Object/Enemy/EnemyBase.h>

//! @brief 初期化
bool SpawnManager::Init()
{
    Super::Init();

    spawn_timer_ = spawn_time_;

    return true;
}

//! @brief 更新
void SpawnManager::Update()
{
    Super::Update();
    auto camera = Scene::Object::Get<Camera>();
    // クリア処理
    if(is_start_clear_)
    {
        ClearAllObjects(true);
        return;
    }

    // スポナーの更新
    for(auto& sd : spawners_data)
    {
        SpawnObjectAtSpawner(sd);
    }

    if(is_stop_)
    {
        return;
    }
    // 初期スポーン
    if(init_spawn_)
    {
        DoSpawn(false, false);
        return;
    }

    // スポーンタイマー更新
    spawn_timer_ -= GetDeltaTime();
    if(spawn_timer_ > 0.0f)
    {
        return;
    }
    spawn_timer_ = spawn_time_;
    // スポーン処理
    DoSpawn();
}

//! @brief 描画
void SpawnManager::LateDraw()
{
    Super::LateDraw();
    // デバッグ描画
    if(is_debug_draw_)
    {
        for(auto& spawner : spawners_data)
        {
            DrawSphere3D(cast(spawner.pos_), 5, 64, GetColor(255, 0, 0), GetColor(255, 0, 0), true);
        }
    }
}

//! @brief 終了
void SpawnManager::Exit()
{
    Super::Exit();
    ClearAllObjects(true);
}

//! @brief GUI表示処理
void SpawnManager::GUI()
{
    Super::GUI();
    ImGui::Begin(GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode(u8"Spawner座標"))
        {
            for(auto& sd : spawners_data)
            {
                ImGui::Text("Pos: (%.2f, %.2f, %.2f)", (float)sd.pos_.x, (float)sd.pos_.y, (float)sd.pos_.z);
            }
            ImGui::TreePop();
        }
        ImGui::Checkbox(u8"デバッグ描画", &is_debug_draw_);

        ImGui::Separator();
        ImGui::Text(u8"シーンにあるオブジェクト数: %d", (int)active_objects_.size());
        for(auto& obj : active_objects_)
        {
            ImGui::Text("%s", obj->GetName().data());
        }
        ImGui::Separator();
        ImGui::Text(u8"プールにあるオブジェクト");
        ImGui::Text(u8"オブジェクト数: %d", (int)object_pool_.size());

        for(auto& obj : object_pool_)
        {
            ImGui::Text("%s", obj->GetName().data());
        }
    }
    ImGui::End();
}

//! @brief スポーンプールに追加
//! @param object オブジェクト
//! @return 自身のポインタ
SpawnManagerPtr SpawnManager::AddSpawnPool(ObjectPtr object)
{
    // 同じのが入っているかチェック
    auto not_same = std::find(object_pool_.begin(), object_pool_.end(), object);
    if(not_same == object_pool_.end())
    {
        object_pool_.push_back(object);
    }
    // アクティブリストから削除
    auto find_active = std::find(active_objects_.begin(), active_objects_.end(), object);
    if(find_active != active_objects_.end())
    {
        active_objects_.erase(find_active);
    }

    // スポーナーがこのオブジェクトを参照している場合はリセット
    for(auto& spawner : spawners_data)
    {
        if(spawner.spawned_object_.lock() == object)
        {
            spawner.spawned_object_.reset();
            break;
        }
    }

    return std::dynamic_pointer_cast<SpawnManager>(shared_from_this());
}

//! @brief スポナーを追加
//! @param pos 座標
//! @return 自身のポインタ
SpawnManagerPtr SpawnManager::AddSpawner(const float3& pos)
{
    // データを追加
    SpawnerData data;
    data.pos_         = pos;
    data.is_in_view_  = false;
    data.is_init_pos_ = false;
    spawners_data.emplace_back(data);
    return std::dynamic_pointer_cast<SpawnManager>(shared_from_this());
}

//! @brief スポナーを追加
//! @param data_path データパス
//! @return 自身のポインタ
SpawnManagerPtr SpawnManager::AddSpawners(std::string_view data_path)
{
    std::ifstream file(data_path.data());
    if(!file.is_open())
    {
        return std::dynamic_pointer_cast<SpawnManager>(shared_from_this());
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
        // データを追加
        float3 pos;
        pos.x = std::stof(values[0]);
        pos.y = std::stof(values[1]);
        pos.z = std::stof(values[2]);
        SpawnerData data;
        data.pos_        = pos;
        data.is_in_view_ = false;
        spawners_data.emplace_back(data);
    }

    file.close();
    return std::dynamic_pointer_cast<SpawnManager>(shared_from_this());
}

//! @brief スポナーを追加
//! @param spawners スポナー座標リスト
//! @return 自身のポインタ
SpawnManagerPtr SpawnManager::AddSpawners(std::vector<float3> spawners)
{
    for(auto& spawner : spawners)
    {
        AddSpawner(spawner);
    }
    return std::dynamic_pointer_cast<SpawnManager>(shared_from_this());
}

//! @brief スポーンを実行
//! @param check_view 画面内チェックを行うか
//! @param check_distance 距離チェックを行うか
void SpawnManager::DoSpawn(bool check_view, bool check_distance)
{
    auto camera = Scene::Object::Get<Camera>();
    auto player = Scene::Object::Get<Player>();
    if(!player || !camera)
    {
        return;
    }
    spawn_timer_ = 0.0f;
    // 最大数に達していたら生成しない
    if(active_objects_.size() >= max_active_objects_)
    {
        return;
    }

    if(object_pool_.empty())
    {
        return;
    }

    // 範囲外のスポーン地点を検出
    std::vector<int> out_of_view_spawners;
    for(int i = 0; i < (int)spawners_data.size(); i++)
    {
        auto& sd       = spawners_data[i];
        sd.is_in_view_ = camera->IsInCameraView(sd.pos_);
        if(check_view && sd.is_in_view_)
        {
            continue;
        }
        // 既に何かがスポーン地点に紐づいているならスキップ
        if(sd.spawned_object_.lock())
        {
            continue;
        }
        out_of_view_spawners.push_back(i);
    }
    if(out_of_view_spawners.empty())
    {
        return;
    }

    // プレイヤーに近い順にソート
    std::sort(out_of_view_spawners.begin(), out_of_view_spawners.end(), [&player, this](int a, int b) {
        float dist_a = length(player->GetTranslate() - this->spawners_data[a].pos_);
        float dist_b = length(player->GetTranslate() - this->spawners_data[b].pos_);
        return dist_a < dist_b;
    });

    for(int spawner : out_of_view_spawners)
    {
        if(active_objects_.size() >= max_active_objects_)
        {
            break;
        }

        if(object_pool_.empty())
        {
            break;
        }

        // 距離チェック
        float distance       = length(player->GetTranslate() - spawners_data[spawner].pos_);
        float spawn_distance = spawn_distance_;
        if(!check_distance)
        {
            spawn_distance = 0.0f;
        }
        if(distance < spawn_distance)
        {
            continue;
        }

        // プールからランダムにオブジェクトを取得
        int random = GetRand((int)object_pool_.size() - 1);
        auto obj   = object_pool_[random];

        // ★座標を先に設定（ResetObjectInfo()より前）
        obj->SetTranslate(spawners_data[spawner].pos_);

        // オブジェクトの初期化
        obj->ResetObjectInfo();
        obj->SetStatus(Object::StatusBit::NoUpdate, false);
        if(auto col = obj->GetComponent<ComponentCollision>())
        {
            col->SetCollisionStatus(ComponentCollision::CollisionBit::DisableHit, true);
        }

        spawners_data[spawner].is_init_pos_    = false;
        spawners_data[spawner].spawned_object_ = obj;

        active_objects_.push_back(obj);

        // プールから削除
        object_pool_.erase(object_pool_.begin() + random);
        if(init_spawn_)
        {
            init_spawn_ = false;
        }
    }
}

void SpawnManager::SpawnProcess()
{
    spawn_timer_ = 0.0f;
    // 最大数に達していたら生成しない
    if(active_objects_.size() >= max_active_objects_)
    {
        return;
    }

    if(object_pool_.empty())
    {
        return;
    }

    for(int i = 0; i < (int)spawners_data.size(); i++)
    {
        if(active_objects_.size() >= max_active_objects_)
        {
            break;
        }

        if(object_pool_.empty())
        {
            break;
        }

        // 既に何かがスポーン地点に紐づいているならスキップ
        if(spawners_data[i].spawned_object_.lock())
        {
            continue;
        }

        // プールからランダムにオブジェクトを取得
        int random = GetRand((int)object_pool_.size() - 1);
        auto obj   = object_pool_[random];

        // ★座標を先に設定（ResetObjectInfo()より前）
        obj->SetTranslate(spawners_data[i].pos_);

        // オブジェクトの初期化
        obj->ResetObjectInfo();
        obj->SetStatus(Object::StatusBit::NoUpdate, false);
        if(auto col = obj->GetComponent<ComponentCollision>())
        {
            col->SetCollisionStatus(ComponentCollision::CollisionBit::DisableHit, true);
        }

        spawners_data[i].is_init_pos_    = false;
        spawners_data[i].spawned_object_ = obj;

        active_objects_.push_back(obj);

        // プールから削除
        object_pool_.erase(object_pool_.begin() + random);
        if(init_spawn_)
        {
            init_spawn_ = false;
        }
    }
}

//! @brief スポーン初期化
void SpawnManager::InitSpawn()
{
    init_spawn_ = true;
}

//! @brief スポーン停止
//! @param is_stop 停止するか
void SpawnManager::StopSpawn(bool is_stop)
{
    is_stop_ = is_stop;
}

//! @brief シーン内の最大オブジェクト数を設定
//! @param max_objects 最大数
void SpawnManager::SetSceneObjectMax(int max_objects)
{
    max_active_objects_ = max_objects;
}

//! @brief スポーン済みオブジェクトを設定
//! @param obj オブジェクト
//! @param index インデックス
void SpawnManager::SetSpawnedObject(ObjectPtr obj, int index)
{
    spawners_data[index].spawned_object_ = obj;
}

//! @brief スポーン済みオブジェクトを取得
//! @param index インデックス
//! @return オブジェクト
ObjectPtr SpawnManager::GetSpawnedObject(int index) const
{
    return spawners_data[index].spawned_object_.lock();
}

//! @brief プールオブジェクトをクリア
//! @param is_destroy_all 全て削除するか
void SpawnManager::ClearPoolObjects(bool is_destroy_all)
{
    if(is_destroy_all)
    {
        for(auto& obj : object_pool_)
        {
            Scene::Object::Release(obj);
        }
    }
    object_pool_.clear();
}

//! @brief 全てのオブジェクトをクリア
//! @param remove_self 自身を削除するか
void SpawnManager::ClearAllObjects(bool remove_self)
{
    ClearPoolObjects(true);
    if(remove_self && active_objects_.empty() && object_pool_.empty())
    {
        Scene::Object::Release(shared_from_this());
    }
}

//! @brief オブジェクトクリア開始
void SpawnManager::StartClearObjects()
{
    is_start_clear_ = true;
    is_stop_        = true;
}

//! @brief 全てのスポナーを削除
void SpawnManager::RemoveAllSpawners()
{
    spawners_data.clear();
}

//! @brief スポナーでオブジェクトをスポーン
//! @param spawner スポナーデータ
void SpawnManager::SpawnObjectAtSpawner(SpawnerData& spawner)
{
    if(auto obj = spawner.spawned_object_.lock())
    {
        // スポナーからの距離を取得
        float3 vec        = obj->GetTranslate() - spawner.pos_;
        float distance_sq = dot(vec, vec);
        float distance    = sqrtf(distance_sq);
        // 位置まだ初期化していない場合
        if(!spawner.is_init_pos_)
        {
            const float INIT_DISTANCE = 10.0f;
            // 距離が近ければ初期化完了
            if(distance < INIT_DISTANCE)
            {
                obj->SetTranslate(spawner.pos_);
                spawner.is_init_pos_ = true;
            }
            else
            {
                // 位置合わせを続ける
                obj->SetStatus(Object::StatusBit::NoDraw, true);
                obj->SetTranslate(spawner.pos_);
            }
        }
        else
        // 初期化完了後
        {
            // 描画と当たり判定を有効化
            obj->SetStatus(Object::StatusBit::NoDraw, false);
            if(auto col = obj->GetComponent<ComponentCollision>())
            {
                col->SetCollisionStatus(ComponentCollision::CollisionBit::DisableHit, false);
            }
            else
            {
                obj->ResetObjectInfo();
            }
            // 生成したオブジェクトは自分と離脱距離以上離れたらスポナーから外す
            if(distance > EXPIRED_DISTANCE_)
            {
                spawner.spawned_object_.reset();
            }
        }
    }
}

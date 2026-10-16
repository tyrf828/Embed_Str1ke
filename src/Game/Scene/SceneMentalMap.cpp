//===========================================================================
//! @file SceneMentalMap.cpp
//! @brief メンタルマップシーン
//===========================================================================
#include "SceneMentalMap.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionModel.h>
#include <Game/Object/Tool/AddData.h>
#include <Game/Object/Tool/MentalMapBuilder.h>
#include <System/Debug/DebugCamera.h>
#include <Game/Component/Navmesh.h>
#include <System/Component/ComponentCollisionLine.h>
#include <System/Component/ComponentCamera.h>
#include <Game/Object/Enemy/MentalMap.h>
#include <Game/Object/Manager/SpawnManager.h>
#include <Game/Object/Enemy/CloseEnemy/CloseEnemy.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyAction.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyIdle.h>
#include <System/Component/ComponentCollisionCapsule.h>

#include <Game/Object/Stage.h>

//! @brief 初期化
//! @return 初期化済み
bool SceneMentalMap::Init()
{
    Super::Init();

    Scene::Object::Create<MentalMapBuilder>("MentalMapBuilder", true)->AddComponent<Navmesh>();
    Scene::Object::Create<MentalMap>("MentalMap", true);

    {
        Scene::Object::Create<Stage>("Stage")->SetTranslate(float3{0.0f, 0.0f, 0.0f});
    }

    // カメラ
    auto camera = Scene::Object::Create<Object>("Camera")->SetTranslate(float3{190.0f, 265.0f, -270.0f});
    camera->AddComponent<ComponentCamera>()->SetPerspective(68.0f)->SetPositionAndTarget(float3{0.0f, 0.0f, 0.0f}, float3{0.0f, 0.0f, 1.0f});

    Scene::Object::Create<AddData>();

    auto manager = Scene::Object::Create<SpawnManager>("EnemySpawnManager");
    manager->StopSpawn();
    manager->SetSceneObjectMax(20);
    manager->UseDebugDraw();

    return true;
}

//! @brief 更新
void SceneMentalMap::Update()
{
    Super::Update();

    auto R           = (GetMouseInput() & MOUSE_INPUT_RIGHT);
    float2 mouse_now = float2{GetMouseX(), GetMouseY()};
    mouse_vec_       = mouse_now - mouse_xy_;
    mouse_xy_        = mouse_now;
    if(R)
    {
        auto cam_obj = Scene::Object::Get<Object>("Camera");
        auto cam     = cam_obj->GetComponent<ComponentCamera>();
        // カメラに合わせた軸の作成
        float3 back  = normalize(-cam_obj->GetVectorAxisZ());
        float3 up    = {0, 1, 0};
        float3 right = cross(up, back);
        float3 front = -back;

        float3 cam_rot = cam_obj->GetRotationAxisXYZ() + float3{mouse_vec_.y * 0.3f, mouse_vec_.x * 0.3f, 0.0f};
        cam_rot.x      = clamp(cam_rot.x, -89.0f, 89.0f);

        cam_obj->SetRotationAxisXYZ(cam_rot);

        float speed = 2.0f;
        if(Input::IsKey(KEY_INPUT_LSHIFT))
        {
            speed = 5.0f;
        }

        if(Input::IsKey(KEY_INPUT_W))
        {
            cam_obj->SetTranslate(cam_obj->GetTranslate() + front * speed);
        }
        if(Input::IsKey(KEY_INPUT_A))
        {
            cam_obj->SetTranslate(cam_obj->GetTranslate() + right * speed);
        }
        if(Input::IsKey(KEY_INPUT_D))
        {
            cam_obj->SetTranslate(cam_obj->GetTranslate() - right * speed);
        }
        if(Input::IsKey(KEY_INPUT_S))
        {
            cam_obj->SetTranslate(cam_obj->GetTranslate() + back * speed);
        }
        if(Input::IsKey(KEY_INPUT_Q))
        {
            cam_obj->SetTranslate(cam_obj->GetTranslate() + up * speed);
        }
        if(Input::IsKey(KEY_INPUT_E))
        {
            cam_obj->SetTranslate(cam_obj->GetTranslate() - up * speed);
        }
    }
    if(Input::IsKeyDown(KEY_INPUT_M))
    {
        change_enemy_mode_ = !change_enemy_mode_;
    }

    // カメラの前にレイを飛ばして、カメラの前にナビメッシュのプレイヤーとして認識する
    if(IsKeyDown(KEY_INPUT_SPACE))
    {
        auto cam_obj = Scene::Object::Get<Object>("Camera");
        auto cam     = cam_obj->GetComponent<ComponentCamera>();

        MV1_COLL_RESULT_POLY hit_poly{};
        auto model = Scene::Object::Get<Object>("Stage")->GetComponent<ComponentModel>();
        hit_poly   = MV1CollCheck_Line(
            model->GetModel(), -1, cast(cam_obj->GetTranslate()), cast(cam_obj->GetTranslate() + normalize(cam_obj->GetVectorAxisZ()) * 1000.0f));

        if(hit_poly.HitFlag)
        {
            if(!change_enemy_mode_)
            {
                nav_player_pos_ = cast(hit_poly.HitPosition);
                if(auto builder = Scene::Object::Get<MentalMapBuilder>())
                {
                    builder->SetNavmeshTarget(nav_player_pos_);
                }
                if(auto map = Scene::Object::Get<MentalMap>())
                {
                    map->SetPlayerPos(nav_player_pos_);
                }
            }
            else
            {
                Scene::Object::Create<CloseEnemy>("CloseEnemy");
                if(auto manager = Scene::Object::Get<SpawnManager>("EnemySpawnManager"))
                {
                    manager->AddSpawner(cast(hit_poly.HitPosition) + float3{0.0f, 1.0f, 0.0f});
                    manager->SpawnProcess();
                }
            }
        }
    }
    // 敵が動く
    if(IsKeyDown(KEY_INPUT_P))
    {
        if(auto manager = Scene::Object::Get<SpawnManager>("EnemySpawnManager"))
        {
            manager->RemoveAllSpawners();
        }

        auto mental_map = Scene::Object::Get<MentalMap>();
        if(!mental_map)
        {
            return;
        }

        // 現在のゾーン情報を取得
        MentalMap::MentalZoneInfo zone_info = mental_map->GetMentalZoneInfo();

        for(auto& enemy : Scene::Object::GetArray<CloseEnemy>())
        {
            auto action  = enemy->GetComponent<CloseEnemyAction>();
            auto navmesh = enemy->GetComponent<Navmesh>();

            if(!action || !navmesh)
            {
                continue;
            }

            action->SetAction(std::make_shared<CloseEnemyIdle>());
            if(auto move = enemy->GetComponent<EnemyMove>())
            {
                move->SetSpeed(0.5f);
            }

            float3 enemy_pos  = enemy->GetTranslate();
            float3 target_pos = mental_map->GetEnemyTargetPos(zone_info, enemy_pos);

            navmesh->SetTarget(target_pos);
        }
    }

    if(auto manager = Scene::Object::Get<SpawnManager>("EnemySpawnManager"))
    {
        if(manager->GetSpawnerCount() > 0)
        {
            manager->SpawnProcess();
        }
    }

    // 敵の削除
    if(IsKeyDown(KEY_INPUT_C))
    {
        if(auto manager = Scene::Object::Get<SpawnManager>("EnemySpawnManager"))
        {
            manager->RemoveAllSpawners();
            manager->ClearActiveObjects(true);
        }
    }

    // キー操作説明
    printfDx("\n");
    printfDx("--------------------------------\n");
    printfDx("スペースキー: カメラ前にプレイヤー/敵配置\n");
    printfDx("Mキー: プレイヤー/敵配置モード切替 (現在:%s)\n", change_enemy_mode_ ? "敵配置モード" : "プレイヤー配置モード");
    printfDx("Pキー: 敵を移動させる\n");
    printfDx("Cキー: 敵を全て削除\n");
    printfDx("--------------------------------\n");
}

//! @brief 更新後
void SceneMentalMap::PostUpdate()
{
    Super::PostUpdate();
}

void SceneMentalMap::Draw()
{
    Super::Draw();
    SetUseLighting(false);
    DrawSphere3D(cast(nav_player_pos_), 5.0f, 32, GetColor(0, 255, 0), GetColor(0, 255, 0), false);

#if 1
    if(auto manager = Scene::Object::Get<SpawnManager>("EnemySpawnManager"))
    {
        if(manager->GetSpawnerCount() <= 0)
        {
            SetUseLighting(true);
            return;
        }
    }
    for(auto& enemy : Scene::Object::GetArray<CloseEnemy>())
    {
        auto mental_map                     = Scene::Object::Get<MentalMap>();
        MentalMap::MentalZoneInfo zone_data = mental_map->GetMentalZoneInfo();

        std::vector<float3> candidate_points;
        float3 enemy_vec  = normalize(enemy->GetTranslate() - nav_player_pos_);
        bool found_target = false;

        // Zone0がまだ空いているか見る
        if(zone_data.zone0_.object_ < zone_data.zone0_.max_object_)
        {
            // 空いていればそこに配置
            for(const auto& point : zone_data.zone0_.points_)
            {
                float distance = length(point - nav_player_pos_);
                float radius   = 30.0f;

                if(distance < radius)
                {
                    continue;
                }
                float3 point_vec = normalize(point - nav_player_pos_);

                // ドット積を角度に変換して判定
                float dot_val   = dot(enemy_vec, point_vec);
                float dot_angle = acosf(dot_val) * RadToDeg;

                if(dot_angle <= 30.0f)
                {
                    candidate_points.push_back(point);
                    found_target = true;
                }
            }
        }

        if(!found_target)
        {
            for(auto& zone1 : zone_data.zone1_)
            {
                if(zone1.object_ >= zone1.max_object_)
                {
                    continue;
                }

                for(const auto& point : zone1.points_)
                {
                    float distance = length(point - nav_player_pos_);
                    float radius   = 30.0f;

                    if(distance < radius)
                    {
                        continue;
                    }
                    float3 point_vec = normalize(point - nav_player_pos_);

                    float dot_val   = dot(enemy_vec, point_vec);
                    float dot_angle = acosf(dot_val) * RadToDeg;

                    if(dot_angle <= 60.0f)
                    {
                        candidate_points.push_back(point);
                        found_target = true;
                    }
                }
            }
        }

        if(!found_target && candidate_points.empty())
        {
            // Zone2の検索
            float min_dist_to_zone2           = FLT_MAX;
            MentalMap::ZoneData* target_zone2 = nullptr;

            for(auto& zone2 : zone_data.zone2_)
            {
                if(zone2.object_ >= zone2.max_object_)
                {
                    continue;
                }

                // Zone2の中心点
                float3 center = float3{0, 0, 0};
                if(zone2.points_.empty())
                {
                    continue;
                }

                for(const auto& p : zone2.points_)
                {
                    center += p;
                }
                center /= (float)zone2.points_.size();

                float3 diff = center - enemy->GetTranslate();
                float dist  = dot(diff, diff);
                if(dist < min_dist_to_zone2)
                {
                    min_dist_to_zone2 = dist;
                    target_zone2      = &zone2;
                }
            }

            if(target_zone2)
            {
                // 見つけたZone2からターゲットを選ぶ
                if(!target_zone2->points_.empty())
                {
                    for(const auto& p : target_zone2->points_)
                    {
                        candidate_points.push_back(p);
                    }
                    found_target = true;
                }
            }
        }

        for(auto p : candidate_points)
        {
            DrawSphere3D(cast(p), 5.0f, 32, GetColor(255, 0, 0), GetColor(255, 0, 0), false);
        }
    }
#endif
    SetUseLighting(true);
}

//! @brief 描画
void SceneMentalMap::LateDraw()
{
    Super::LateDraw();

    int color = change_enemy_mode_ ? GetColor(255, 0, 0) : GetColor(0, 255, 0);

    // レティクル
    DrawCircleAA(WINDOW_W * 0.5f, WINDOW_H * 0.5f, 4.0f, 32, color, false);
}

//! @brief 終了
void SceneMentalMap::Exit()
{
    Super::Exit();
}

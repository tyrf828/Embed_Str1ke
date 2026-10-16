//===========================================================================
//! @file SceneBoss.cpp
//! @brief ボス戦シーン
//===========================================================================
#include "SceneBoss.h"
#include <System/Component/ComponentModel.h>
#include <Game/Object/Player/Player.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Object/Weapon/Stick.h>
#include <Game/Object/Player/GameUI.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Object/Stage.h>
#include <Game/Object/Enemy/Boss/Boss.h>
#include <Game/Object/BossStage.h>
#include <Game/Object/Manager/SpawnManager.h>
#include <Game/Component/SphericalMaskTransition.h>
#include <System/Component/ComponentCollisionModel.h>
#include <Game/Object/Spawner.h>
#include <Game/Object/Weapon/AttachWeapon/Barrel.h>
#include <Game/Object/Weapon/AttachWeapon/TrafficCone.h>
#include <Game/Object/Weapon/AttachWeapon/BarrelWet.h>
#include <Game/Object/Weapon/AttachWeapon/Box.h>
#include <Game/Scene/SceneTitle.h>
#include <Game/Component/Sound.h>
#include <Game/Scene/SceneVictoryCutScene.h>
#include <Game/Object/SkyDome/BlueSkyDome.h>
#include <Game/Object/SkyDome/BossSkyDome.h>
#include <Game/Object/LightPoint.h>
#include <Game/Component/CircularBoundary.h>
#include <Game/Object/Sun.h>
#include <Game/Object/FilterVolumetricFog.h>

//! @brief 初期化
//! @return 初期化済み
bool SceneBoss::Init()
{
    Super::Init();

    // 太陽
    Scene::CreateObjectPtr<Sun>()->SetTranslate(float3{700.0f, 700.0f, -1000.0f});
    volumetric_fog_ = Scene::Object::Get<FilterVolumetricFog>();
    if(auto fog = volumetric_fog_.lock())
    {
        fog->SetEnable(false);
    }
    // 前のステージ
    {
        const float3 spawn_pos = float3{-764.0f, -92.0f, 52.0f};
        auto last_stage        = Scene::Object::Create<Stage>("Stage")->SetTranslate(spawn_pos)->SetRotationAxisXYZ({0.0f, 180.0f, 0.0f});

        // コリジョンを削除
        if(last_stage->GetComponent<ComponentCollisionModel>())
        {
            last_stage->RemoveComponent<ComponentCollisionModel>();
        }

        // 球体マスク遷移
        auto spherical = last_stage->AddComponent<SphericalMaskTransition>()->SetMaskInverse(true);
        spherical->SetDestroyLimit(600.0f, true);
    }
    {
        auto sky       = Scene::Object::Create<BlueSkyDome>("Sky");
        auto spherical = sky->AddComponent<SphericalMaskTransition>()->SetMaskInverse(true);
        spherical->SetDestroyLimit(700.0f, true);
    }

    // ボスステージ
    {
        auto stage     = Scene::Object::Create<BossStage>("BossStage")->SetTranslate(float3(0.0f, 5.0f, 0.0f));
        auto spherical = stage->AddComponent<SphericalMaskTransition>()->SetMaskInverse(false);
        spherical->SetDestroyLimit(700.0f, false);
    }
    {
        auto sky       = Scene::Object::Create<BossSkyDome>("Sky");
        auto spherical = sky->AddComponent<SphericalMaskTransition>()->SetMaskInverse(false);
        spherical->SetDestroyLimit(700.0f, false);
        spherical->StartTransition();
    }

    // プレイヤの初期設定
    auto player = Scene::Object::Create<Player>("Player");
    {
        auto model = player->GetComponent<ComponentModel>();
        // プレイヤモデルの向き
        model->SetRotationAxisXYZ({0, 0, 0});
        // プレイヤの座標設定
        player->SetTranslate({0.0f, 7.0f, 60.0f});
    }
    Scene::Object::Create<Stick>("Stick");

    Scene::Object::Create<GameUI>("GameUI", true);

    // ボス
    auto boss = Scene::Object::Create<Boss>()->SetTranslate({0, 10, -30})->SetRotationAxisXYZ(float3{0, 180, 0});

    // アイテムスポナーの設定
    auto manager = Scene::Object::Create<SpawnManager>("SpawnManager", true)->AddSpawners("data/Game/SpawnerData/SceneBossItemSpawner.txt");
    manager->SetSpawnDistance(80.0f);
    manager->SetSpawnTime(6.0f);
    manager->SetSceneObjectMax(8);

    const int ITEM_SPAWN_COUNT = 3;
    for(int i = 0; i < ITEM_SPAWN_COUNT; i++)
    {
        manager->AddSpawnPool(Scene::Object::Create<TrafficCone>());
        manager->AddSpawnPool(Scene::Object::Create<Barrel>());
        manager->AddSpawnPool(Scene::Object::Create<BarrelWet>());
        manager->AddSpawnPool(Scene::Object::Create<Box>());
    }
    manager->InitSpawn();

    // カメラ
    Scene::Object::Create<Camera>("Camera");

    // プレイヤとボスを閉じ込める
    auto boundary = Scene::Object::Create<Object>("CircularBoundary")->AddComponent<CircularBoundary>()->AddBoundaryObject(player)->AddBoundaryObject(boss);
    boundary->SetRadius(125.0f);

    // BGM再生
    auto sound = Scene::Object::Create<Object>("BGM", true)->AddComponent<Sound>();
    sound->Load2D({
        {"bgm", "data/Game/BGM/inferior_diety_v2.ogg", 0.0f, 0.7f, Sound::TYPE::MUSIC},
    });
    sound->Play2D("bgm", DX_PLAYTYPE_LOOP);

    return true;
}

//! @brief 更新
void SceneBoss::Update()
{
    Super::Update();

    // ボスが存在しない場合は勝利シーンへ遷移
    auto boss = Scene::Object::Get<Boss>();
    if(!boss)
    {
        Scene::Change(Scene::GetScene<SceneVictoryCutScene>());
        return;
    }

    // ボスのHPが0になったらライトを消す
    if(auto hp = boss->GetComponent<ComponentHP>())
    {
        if(hp->GetHP() <= 0)
        {
            auto lights = Scene::Object::GetArray<LightPoint>();
            for(auto& light : lights)
            {
                Scene::Object::Release(light);
            }
            return;
        }
    }

    // 時間経過でライトを生成する演出
    scene_light_time_ += GetDeltaTime();
    if(scene_light_time_ >= LIGHT_SPAWN_TIME_)
    {
        if(!is_light_spawned_[0])
        {
            SpawnBottomLightPoint();
            is_light_spawned_[0] = true;
            Scene::Object::Release(Scene::Object::Get<Sun>());
        }

        if(is_light_spawned_[0] && !is_light_spawned_[1])
        {
            if(scene_light_time_ >= LIGHT_SPAWN_TIME_ + TOP_LIGHT_DELAY_)
            {
                SpawnTopLightPoint();
                is_light_spawned_[1] = true;
            }
        }
    }
}

//! @brief 終了
void SceneBoss::Exit()
{
    Super::Exit();
    if(auto fog = volumetric_fog_.lock())
    {
        fog->SetEnable(true);
    }
    volumetric_fog_.reset();
}

//! @brief 下のライト生成
void SceneBoss::SpawnBottomLightPoint()
{
    const int LIGHT_POINT_COUNT = 4;
    for(int i = 0; i < LIGHT_POINT_COUNT; i++)
    {
        float angle = (360.0f / LIGHT_POINT_COUNT) * i * DegToRad;
        float x     = 145.0f * sinf(angle);
        float z     = 145.0f * cosf(angle);
        auto point  = Scene::Object::Create<LightPoint>();
        point->SetTranslate(float3(x, 18.0f, z));
        point->Set(float4(1.0f, 1.0f, 1.0f, 1.0f), 100.0f, 100.0f);
    }
}

//! @brief 上のライト生成
void SceneBoss::SpawnTopLightPoint()
{
    const int LIGHT_POINT_COUNT = 4;
    for(int i = 0; i < LIGHT_POINT_COUNT; i++)
    {
        float angle = ((360.0f / LIGHT_POINT_COUNT) * i + 45.0f) * DegToRad;
        float x     = 145.0f * sinf(angle);
        float z     = 145.0f * cosf(angle);
        auto point  = Scene::Object::Create<LightPoint>();
        point->SetTranslate(float3(x, 95.0f, z));
        point->Set(float4(1.0f, 1.0f, 1.0f, 1.0f), 100.0f, 100.0f);
    }
}

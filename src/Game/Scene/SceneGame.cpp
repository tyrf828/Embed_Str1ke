//===========================================================================
//! @file SceneGame.cpp
//! @brief ゲームシーン
//===========================================================================
#include "SceneGame.h"
#include <System/Component/ComponentModel.h>
#include <Game/Object/Player/Player.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Object/Enemy/CloseEnemy/CloseEnemy.h>
#include <Game/Object/Weapon/Stick.h>
#include <Game/Object/Manager/EnemyActionManager.h>
#include <Game/Object/Player/GameUI.h>
#include <Game/Object/Enemy/FarEnemy/FarEnemy.h>
#include <Game/Object/Stage.h>
#include <Game/Object/Manager/SpawnManager.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCollisionBox.h>
#include <Game/Object/Weapon/AttachWeapon/TrafficCone.h>
#include <Game/Object/Weapon/AttachWeapon/Barrel.h>
#include <Game/Object/Weapon/AttachWeapon/BarrelWet.h>
#include <Game/Object/Weapon/AttachWeapon/Box.h>
#include <Game/Object/Manager/SceneGameManager.h>
#include <Game/Component/Tutorial/Tutorial.h>
#include <System/Utils/IniFileLib.h>
#include <Game/Object/StageTrigger.h>
#include <Game/Component/Sound.h>
#include <Game/Object/SkyDome/BlueSkyDome.h>
#include <Game/Object/Sun.h>
#include <Game/Object/BlackScreen.h>
#include <Game/Object/Manager/EnemyMentalManager.h>
#include <Game/Object/Enemy/MentalMap.h>
#include <Game/Object/FilterVolumetricFog.h>

//! @brief 初期化
//! @return 初期化済み
bool SceneGame::Init()
{
    Super::Init();

    Scene::Object::Create<BlackScreen>("BlackScreen", true)->StartFadeOut(true);

    // 太陽
    Scene::CreateObjectPtr<Sun>()->SetTranslate(float3{700.0f, 700.0f, -1000.0f});

    volumetric_fog_ = Scene::Object::Get<FilterVolumetricFog>();
    if(auto fog = volumetric_fog_.lock())
    {
        fog->SetHeight(80.0f);
    }

    // 地面
    Scene::Object::Create<Stage>("Stage")->SetTranslate(float3{0.0f, 0.0f, 0.0f});

    // 壁作成用ラムダ式
    auto make_wall = [](float3 translate, float3 size, std::string_view name) {
        auto obj = Scene::Object::Create<Object>(name)->SetTranslate(translate);
        obj->AddComponent<ComponentCollisionBox>()
            ->SetSize(size)
            ->SetMass(-1.0f)
            ->SetCollisionGroup(ComponentCollision::CollisionGroup::GROUND)
            ->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER | (u32)ComponentCollision::CollisionGroup::ETC |
                                   (u32)ComponentCollision::CollisionGroup::ITEM)
            ->UseGravity(false);
        return obj;
    };

    // 壁
    auto stage_wall = make_wall(float3{-147.5f, 140.0f, -290.0f}, float3{80.0f, 80.0f, 200.0f}, "StageWall");
    auto wall       = make_wall(float3{-760.0f, 125.0f, -160.0f}, float3{50.0f, 50.0f, 200.0f}, "BossWall");

    // プレイヤ
    auto player = Scene::Object::Create<Player>("Player");
    // プレイヤの初期化設定
    {
        auto model = player->GetComponent<ComponentModel>();
        auto col   = player->GetComponent<ComponentCollisionCapsule>();

        model->SetRotationAxisXYZ({0.0f, -90.0f, 0.0f});
        player->SetTranslate({-91.0f, 100.0f, -283.0f});
        col->setGravity(col->gravity() * 3.0f);
    }

    // カメラ
    auto camera = Scene::Object::Create<Camera>("Camera");

    // プレイヤの武器
    Scene::Object::Create<Stick>();

    Scene::Object::Create<BlueSkyDome>("Sky");

    Scene::Object::Create<GameUI>("GameUI", true);

    Scene::Object::Create<EnemyActionManager>("EnemyActionManager", true);

    Scene::Object::Create<MentalMap>("MentalMap", true);
    Scene::Object::Create<EnemyMentalManager>("EnemyMentalManager", true);

    // プレイヤのアタッチ用武器
    auto item_spawner          = Scene::Object::Create<SpawnManager>("ItemSpawnManager", true)->AddSpawners("data/Game/SpawnerData/SceneGameItemSpawner.txt");
    const int ITEM_SPAWN_COUNT = 7;
    for(int i = 0; i < ITEM_SPAWN_COUNT; i++)
    {
        item_spawner->AddSpawnPool(Scene::Object::Create<TrafficCone>());
        item_spawner->AddSpawnPool(Scene::Object::Create<Barrel>());
        item_spawner->AddSpawnPool(Scene::Object::Create<BarrelWet>());
        item_spawner->AddSpawnPool(Scene::Object::Create<Box>());
    }
    // 最初からステージに配置
    item_spawner->InitSpawn();

    // 敵スポーンマネージャー
    auto enemy_spawn_manager = Scene::Object::Create<SpawnManager>("EnemySpawnManager", true)->AddSpawners("data/Game/SpawnerData/SceneGameEnemySpawner.txt");
    enemy_spawn_manager->SetSpawnTime(5.0f);
    enemy_spawn_manager->SetSceneObjectMax(10);
    for(int i = 0; i < 15; i++)
    {
        auto e = Scene::Object::Create<CloseEnemy>();
        enemy_spawn_manager->AddSpawnPool(e);
    }
    for(int i = 0; i < 5; i++)
    {
        auto e = Scene::Object::Create<FarEnemy>();
        enemy_spawn_manager->AddSpawnPool(e);
    }

    // ゲームシーンマネージャー
    Scene::Object::Create<SceneGameManager>("SceneGameManager", true)
        ->SetBossWall(wall)
        ->SetPlayer(player)
        ->AddSpawner(item_spawner)
        ->AddSpawner(enemy_spawn_manager);

    // BGM再生
    auto sound = Scene::Object::Create<Object>("BGM", true)->AddComponent<Sound>();
    sound->Load2D({
        {"bgm", "data/Game/BGM/2010_June_HypnoticChill_17.mp3", 0.0f, 0.7f, Sound::TYPE::MUSIC},
    });
    sound->Play2D("bgm", DX_PLAYTYPE_LOOP);

    return true;
}

//! @brief 更新
void SceneGame::Update()
{
    Super::Update();
    if(auto player = Scene::Object::Get<Object>("Player"))
    {
        if(player->GetTranslate().y < -100.0f)
        {
            // シーン再読み込み
            Scene::Change(Scene::GetScene<SceneGame>());
        }
    }
}

//! @brief 終了
void SceneGame::Exit()
{
    Super::Exit();

    if(auto fog = volumetric_fog_.lock())
    {
        fog->SetHeight();
    }
    volumetric_fog_.reset();
}

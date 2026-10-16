#include "SceneTest.h"
#include <System/Component/ComponentModel.h>
#include <Game/Object/FilterInkWash.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCamera.h>
#include <Game/Object/Player/Player.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Object/Enemy/CloseEnemy/CloseEnemy.h>
#include <Game/Component/ReciprocalVelocityObstacles.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Object/Weapon/Stick.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyAction.h>
#include <Game/Object/Weapon/AttachWeapon/TrafficCone.h>
#include <Game/Object/Manager/EnemyActionManager.h>
#include <Game/Object/Player/GameUI.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyFindPlayer.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyDeath.h>
#include <Game/Component/Navmesh.h>
#include <Game/Object/Enemy/FarEnemy/FarEnemy.h>
#include <Game/Object/Enemy/FarEnemy/EnemyGun.h>
#include <Game/Object/Stage.h>
#include <Game/Object/Enemy/Boss/Boss.h>
#include <Game/Object/TestStage.h>
#include <Game/Object/Effect/SlashEffect.h>
#include <Game/Object/BossStage.h>
#include <Game/Object/Manager/SpawnManager.h>
#include <System/Component/ComponentCollisionBox.h>
#include <Game/Object/Spawner.h>
#include <Game/Component/Weapon/AttachWeaponStandby.h>
#include <Game/Object/Tool/AddData.h>
#include <Game/Object/Weapon/AttachWeapon/Barrel.h>
#include <Game/Component/DissolveEffect.h>
#include <Game/Object/Weapon/AttachWeapon/Box.h>
#include <Game/Object/Weapon/AttachWeapon/BarrelWet.h>
#include <Game/Component/OutLine.h>
#include <System/Component/ComponentCollisionLine.h>
#include <Game/Object/Tool/MentalPosBall.h>

#include <Game/Object/FilterLensFlare.h>
#include <Game/Object/Sun.h>
#include <Game/Component/Decal.h>
#include <Game/Object/Effect/FloorBreakEffect.h>
#include <Game/Component/Weapon/AttachWeaponParts.h>
#include <Game/Component/Weapon/AttachWeapon.h>
#include <Game/Object/Effect/InkSplash.h>
#include <Game/Component/Enemy/Boss/BossAI.h>

bool SceneTest::Init()
{
    Super::Init();

    Scene::Object::Create<Object>("AI")->AddComponent<BossAI>();

    {
        //auto sun   = Scene::CreateObjectPtr<Sun>()->SetTranslate(float3{0.0f, 100.0f, 0.0f});
        auto decal = Scene::Object::Create<FloorBreakEffect>("Decal")->SetScaleAxisXYZ(float3{100.0f, 100.0f, 100.0f});

        // 地面
        {
            auto stage   = Scene::Object::Create<TestStage>("Stage")->SetTranslate(float3{0.0f, 0.0f, 0.0f});
            auto m_stage = stage->GetComponent<ComponentModel>();
            m_stage->UseReceivesDecal(1);
        }
    }

    // プレイヤー
    {
        auto player = Scene::Object::Create<Player>("Player");
        auto model  = player->GetComponent<ComponentModel>();
        // プレイヤーモデルの向き
        model->SetRotationAxisXYZ({0.0f, 150.0f, 0.0f});
        // プレイヤーの座標設定
        player->SetTranslate({10.0f, 25.0f, -20.0f});
        //player;
        //player->AddComponent<OutLine>();
    }
    // カメラ
    Scene::Object::Create<Camera>("Camera");
    auto s = Scene::Object::Create<Stick>();
    s->DontDestroyOnLoad();
    //s->GetComponent<ComponentCollisionCapsule>()->SetHeight(300000);

    Scene::Object::Create<Barrel>()->SetTranslate({0, 10, -30});
    Scene::Object::Create<Box>()->SetTranslate({0, 10, -50});
    Scene::Object::Create<BarrelWet>()->SetTranslate({0, 10, -30});
    Scene::Object::Create<Barrel>()->SetTranslate({0, 10, -30});
    Scene::Object::Create<Box>()->SetTranslate({0, 10, -50});
    Scene::Object::Create<BarrelWet>()->SetTranslate({0, 10, -30});
    Scene::Object::Create<Barrel>()->SetTranslate({0, 10, -30});
    Scene::Object::Create<Box>()->SetTranslate({0, 10, -50});
    Scene::Object::Create<BarrelWet>()->SetTranslate({0, 10, -30});
    auto cone = Scene::Object::Create<TrafficCone>();
    //auto c    = Scene::Object::Create<TrafficCone>()->SetTranslate({30, 10, -30});

    auto sp = Scene::Object::Create<SpawnManager>("EnemySpawnManager");
    for(int i = 0; i < 2; i += 1)
    {
        float start_x  = 150 * sinf(135 + 45 * i * DegToRad);
        float start_z  = 150 * cosf(135 + 45 * i * DegToRad);
        goal_pos_[i].x = -150 * sinf(135 + 45 * i * DegToRad);
        goal_pos_[i].z = -150 * cosf(135 + 45 * i * DegToRad);
        if(i < 8)
        {
            auto e = Scene::Object::Create<CloseEnemy>();
        }
        sp->AddSpawner(float3{start_x, 30, start_z});
        //sp->AddSpawner(float3{60, -100, -145});
    }
    sp->InitSpawn();

    {
        auto sky   = Scene::Object::Create<Object>("Sky");
        auto model = sky->AddComponent<ComponentModel>("data/Game/Model/Skydome/Skydome.mv1")->SetScaleAxisXYZ(float3{2.5, 2.5, 2.5});
        model->RemoveZBuffer();
    }

    Scene::Object::Create<EnemyActionManager>("EnemyActionManager");
    Scene::Object::Create<GameUI>("GameUI");

    {
        Scene::Object::Create<SpawnManager>("EnemySpawnManager")->AddSpawner(float3{-30, 15, -90});
    }
    {
        auto is = Scene::Object::Create<SpawnManager>("ItemSpawnManager")

                      ->AddSpawner(float3{60, 15, 90})
                      ->AddSpawner(float3{50, 15, 90})
                      ->AddSpawner(float3{40, 15, 90})
                      ->AddSpawner(float3{30, 15, 90})
                      ->AddSpawner(float3{20, 15, 90})
                      ->AddSpawner(float3{10, 15, 90})
                      ->AddSpawner(float3{0, 15, 90});
        is->InitSpawn();
    }

    Scene::Object::Create<AddData>();

    //const float range = 100;
    //const int num     = 10;
    //for(int i = 0; i < num; i++)
    //{
    //    for(int j = 0; j < num; j++)
    //    {
    //        auto obj = Scene::Object::Create<MentalPosBall>();
    //        test_balls.push_back(obj);
    //        obj->SetTranslate(float3{-range + i * range / num, 50, -range + j * range / num});
    //    }
    //}

    return true;
}
bool use_forward = false;
void SceneTest::Update()
{
    Super::Update();

    if(IsKeyDown(KEY_INPUT_0))
    {
        auto stick  = Scene::Object::Get<Stick>();
        auto attach = stick->GetComponent<AttachWeaponParts>();
        // 全部dont destroy
        for(auto& part : attach->GetParts())
        {
            part->DontDestroyOnLoad();
            auto attach_weapon = part->GetComponent<AttachWeapon>();
            attach_weapon->GetWeaponJoint()->DontDestroyOnLoad();
        }
    }

    auto enemies = Scene::Object::GetArray<CloseEnemy>();
    for(int i = 0; i < enemies.size(); i++)
    {
        if(IsKeyOn(KEY_INPUT_T))
        {
            if(auto e = enemies[i]->GetComponent<EnemyMove>())
            {
                enemies[i]->RemoveComponent<Navmesh>();
                e->SetSpeed(0.4f);
                //e->SetPosTarget(goal_pos[i]);
                float3 pos  = {0, 0, 0};
                float3 dir  = pos - enemies[i]->GetTranslate();
                dir.x      *= 2.0f;
                dir.z      *= 2.0f;
                //e->SetPosTarget(dir);
                enemies[i]->SetTranslate(Scene::Object::Get<Player>()->GetTranslate());
                //e->SetTarget(Scene::Object::Get<Player>());
                //printfDx("Enemy %d: %0.2f, %0.2f, %0.2f\n", i, (float)e->GetVelocity().x, (float)e->GetVelocity().y, (float)e->GetVelocity().z);
            }
        }
    }

    if(IsKeyOn(KEY_INPUT_RETURN))
    {
        use_forward = !use_forward;
        auto t      = Scene::Object::GetArray<TrafficCone>();
        //BarrelWet
        for(auto obj : Scene::Object::GetArray<BarrelWet>())
        {
            auto model = obj->GetComponent<ComponentModel>();
            if(model)
            {
                model->UseForwardRendering(use_forward);
            }
        }
        for(auto obj : Scene::Object::GetArray<Barrel>())
        {
            auto model = obj->GetComponent<ComponentModel>();
            if(model)
            {
                model->UseForwardRendering(use_forward);
            }
        }
        for(auto obj : Scene::Object::GetArray<Box>())
        {
            auto model = obj->GetComponent<ComponentModel>();
            if(model)
            {
                model->UseForwardRendering(use_forward);
            }
        }
        for(auto obj : t)
        {
            auto model = obj->GetComponent<ComponentModel>();
            if(model)
            {
                model->UseForwardRendering(use_forward);
            }
        }
    }
}

void SceneTest::PostUpdate()
{
    Super::PostUpdate();
}

void SceneTest::LateDraw()
{
    Super::LateDraw();
}

void SceneTest::Exit()
{
    Super::Exit();
}

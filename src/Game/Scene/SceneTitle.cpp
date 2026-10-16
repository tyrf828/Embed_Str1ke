//===========================================================================
//! @file SceneTitle.cpp
//! @brief タイトルシーン
//===========================================================================
#include "SceneTitle.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Object/Tool/AddData.h>
#include <System/Utils/IniFileLib.h>
#include <System/Component/ComponentCamera.h>
#include <Game/Object/Truck.h>
#include <Game/Object/Bridge.h>
#include <Game/Object/Manager/SceneTitleManager.h>
#include <Game/Component/LineTransition.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
//! @return 初期化済み
bool SceneTitle::Init()
{
    Super::Init();

    // プレイヤーデータリセット
    ResetPlayerData();

    {
        auto sky   = Scene::Object::Create<Object>("Sky")->SetTranslate(float3{0.0f, -500.0f, 0.0f});
        auto model = sky->AddComponent<ComponentModel>("data/Game/Model/Skydome/Skydome.mv1")->SetScaleAxisXYZ(float3{3.5f, 3.5f, 3.5f});
        model->RemoveZBuffer();
    }

    // トラック
    auto truck = Scene::Object::Create<Truck>("Truck");
    truck->SetTranslate(float3{-77.5f, 0.0f, -80.0f})->SetRotationAxisXYZ(float3{0.0f, 90.0f, 0.0f});

    // 画面全体トランジション用のオブジェクトを作成
    auto transition_obj  = Scene::Object::Create<Object>("ScreenTransition", true);
    auto line_transition = transition_obj->AddComponent<LineTransition>();

    const int BRIDGE_NUM = 2;
    std::array<BridgePtr, BRIDGE_NUM> bridges;
    for(int i = 0; i < BRIDGE_NUM; i++)
    {
        bridges[i] = Scene::Object::Create<Bridge>("Bridge");
        bridges[i]->SetTranslate(float3{i * -3100.0f, -150.0f, 30.0f});
    }

    // プレイヤがトラックに座っている状態で配置
    auto player = Scene::Object::Create<Object>("Player");
    {
        player->SetTranslate(float3{-32.75f, 16.0f, -75.0f})->SetRotationAxisXYZ(float3{0.0f, -90.0f, 0.0f})->SetScaleAxisXYZ(float3{1.3f, 1.3f, 1.3f});
        auto model = player->AddComponent<ComponentModel>("data/Game/Model/Player/Arissa.mv1")
                         ->SetAnimation({
                             {"sit", "data/Game/Model/Player/Sitting.mv1", 0, 1.0f},
        });
        model->SetOnPlaceY(true);
        model->PlayAnimationNoSame("sit", true);
        player->AddComponent<ComponentCollisionCapsule>()
            ->SetCollisionGroup(ComponentCollision::CollisionGroup::PLAYER)
            ->SetTranslate(float3{0.0f, -1.3f, 0.0f})
            ->UseGravity(true);
    }

    auto camera = Scene::Object::Create<Object>("Camera")->SetTranslate(float3{0.0f, 22.5f, 0.0f})->SetRotationAxisXYZ(float3{9.5f, 0.0f, 0.0f});
    camera->AddComponent<ComponentCamera>()->SetPositionAndTarget(float3{0.0f, 0.0f, 0.0f}, float3{0.0f, 0.0f, -1.0f})->SetPerspective(40.0f);
    camera->AddComponent<ComponentCollisionSphere>()->SetRadius(0.1f);

    Scene::Object::Create<SceneTitleManager>("SceneTitleManager", true)
        ->SetBridge(bridges)
        ->SetCamera(camera)
        ->SetTruck(truck)
        ->SetPlayer(player)
        ->SetLineTransition(line_transition);

    // BGM再生
    auto sound = Scene::Object::Create<Object>("BGM", true)->AddComponent<Sound>();
    sound->Load2D({
        {"bgm", "data/Game/BGM/TitleBGM.mp3", 0.0f, 0.7f, Sound::TYPE::MUSIC},
    });
    sound->Play2D("bgm", DX_PLAYTYPE_LOOP);

    return true;
}
//! @brief 更新
void SceneTitle::Update()
{
    Super::Update();
    if(auto sky = Scene::Object::Get<Object>("Sky"))
    {
        sky->AddRotationAxisXYZ(float3{0.0f, -0.01f, 0.0f});
    }
    if(auto player = Scene::Object::Get<Object>("Player"))
    {
        if(player->GetTranslate().y < -100.0f)
        {
            // シーン再読み込み
            Scene::Change(Scene::GetScene<SceneTitle>());
        }
    }
}

//! @brief プレイヤーデータリセット
void SceneTitle::ResetPlayerData()
{
    // ファイル読み込み
    IniFileLib ini("Game/PlayerData/PlayerData.ini");
    // HP
    const int MAX_HP = ini.GetInt("Player", "MaxHP", 1000);
    ini.WriteInt("Player", "HP", MAX_HP);
    // 回復
    const int MAX_BOTTLE = ini.GetInt("Player", "BottleMax", 3);
    ini.WriteInt("Player", "Bottle", MAX_BOTTLE);
    // 必殺技ゲージ
    ini.WriteInt("Player", "Ultimate", 0);
}

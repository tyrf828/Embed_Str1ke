//===========================================================================
//! @file SceneVictoryCutScene.cpp
//! @brief プレイヤ勝利カットシーン
//===========================================================================
#include "SceneVictoryCutScene.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentAttachModel.h>
#include <Game/Object/Animation/AnimationPlayer.h>
#include <Game/Object/Animation/AnimationBoss.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Object/Weapon/Stick.h>
#include <Game/Object/Stage.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Component/ComponentCamera.h>
#include <Game/Component/CutScene/CutScene.h>
#include <Game/Object/BossStage.h>
#include <Game/Component/CutScene/Victory/CutScenePlayerPickUp.h>
#include <Game/Component/CutScene/Victory/CutSceneEarthQuake.h>
#include <Game/Component/CutScene/Victory/CutSceneStageFade.h>
#include <Game/Component/CutScene/Victory/CutSceneRunEscape.h>
#include <Game/Component/Sound.h>
#include <Game/Object/Crystal.h>
#include <Game/Object/SkyDome/BossSkyDome.h>
#include <Game/Object/SkyDome/BlueSkyDome.h>
#include <Game/Object/Sun.h>

//! @brief 初期化
//! @return 初期化済み
bool SceneVictoryCutScene::Init()
{
    Super::Init();

    // 太陽
    Scene::CreateObjectPtr<Sun>()->SetTranslate(float3{700.0f, 700.0f, -1000.0f});

    {
        // 地面
        auto stage = Scene::Object::Create<BossStage>();

        Scene::Object::Create<BossSkyDome>("Sky");
    }

    {
        const float3 spawn_pos = float3{-764.0f, -97.0f, 52.0f};
        auto game_stage        = Scene::Object::Create<Stage>("GameStage")->SetTranslate(spawn_pos)->SetRotationAxisXYZ({0.0f, 180.0f, 0.0f});
        game_stage->SetStatus(Object::StatusBit::NoDraw, true);

        Scene::Object::Create<BlueSkyDome>("GameSky")->SetStatus(Object::StatusBit::NoDraw, true);
    }

    // プレイヤー
    auto player = Scene::Object::Create<AnimationPlayer>("Player");

    // カメラ
    auto camera = Scene::Object::Create<Object>("Camera");
    {
        camera->AddComponent<ComponentCamera>()->SetPositionAndTarget(float3{0.0f, 0.0f, 0.0f}, float3{0.0f, 0.0f, -1.0f});
        camera->AddComponent<Sound>()->Load({
            {"earthquake", "data/Game/SE/CutScene/地響き1短.mp3",      256.0f, 0.8f},
            {"run",        "data/Game/SE/Player/革靴で走る.mp3",       128.0f, 0.7f},
            {"breath",     "data/Game/SE/CutScene/haa1.mp3",           256.0f, 1.0f},
            {"climb up",   "data/Game/SE/CutScene/ふすまを閉める.mp3", 128.0f, 0.7f},
        });
    }

    {
        auto crystal = Scene::Object::Create<Crystal>("Crystal");
    }

    // カットシーン制御オブジェクト
    {
        auto cut_scene = Scene::Object::Create<Object>("CutSceneController", true);
        cut_scene->AddComponent<CutScene>()->SetPlayer(player)->SetCamera(camera)->SetState(std::make_shared<CutScenePlayerPickUp>());
    }

    return true;
}

//! @brief 更新
void SceneVictoryCutScene::Update()
{
    Super::Update();

    // カメラにリスナーを設定
    if(auto camera = Scene::Object::Get<Object>("Camera"))
    {
        Set3DSoundListenerPosAndFrontPos_UpVecY(cast(camera->GetTranslate()), cast(-camera->GetVectorAxisZ()));
    }
}

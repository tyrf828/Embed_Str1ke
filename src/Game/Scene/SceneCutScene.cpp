//===========================================================================
//! @file SceneCutScene.cpp
//! @brief ボスカットシーン
//===========================================================================
#include "SceneCutScene.h"
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
#include <Game/Component/CutScene/EnterBoss/CutScenePlayerWalkIn.h>
#include <Game/Object/SkyDome/BlueSkyDome.h>
#include <Game/Object/Sun.h>

//! @brief 初期化
//! @return 初期化済み
bool SceneCutScene::Init()
{
    Super::Init();

    // 太陽
    Scene::CreateObjectPtr<Sun>()->SetTranslate(float3{700.0f, 700.0f, -1000.0f});

    // 地面
    auto stage = Scene::Object::Create<Object>("Stage");
    stage->AddComponent<ComponentModel>("data/Game/Model/Stage/LODMap.mv1");
    stage->AddComponent<ComponentCollisionModel>()->AttachToModel();

    // プレイヤー
    auto player = Scene::Object::Create<AnimationPlayer>("Player");

    // ボス
    auto boss = Scene::Object::Create<AnimationBoss>("Boss");

    // カメラ
    auto camera = Scene::Object::Create<Object>("Camera");
    {
        camera->AddComponent<ComponentCamera>();
    }

    // プレイヤの武器
    auto stick = Scene::Object::Get<Stick>();
    if(!stick)
    {
        stick = Scene::Object::Create<Stick>();
    }
    if(auto attach = stick->GetComponent<ComponentAttachModel>())
    {
        attach->SetAttachRotate(float3{0.0f, 4.0f, 0.0f});
        attach->SetAttachOffset(float3{22.3f, 24.0f, 11.1f});
    }

    Scene::Object::Create<BlueSkyDome>("Sky");

    // カットシーン制御オブジェクト
    {
        auto cut_scene = Scene::Object::Create<Object>("CutSceneController", true);
        cut_scene->AddComponent<CutScene>()->SetPlayer(player)->SetBoss(boss)->SetCamera(camera)->SetState(std::make_shared<CutScenePlayerWalkIn>());
    }

    return true;
}

//! @brief 更新
void SceneCutScene::Update()
{
    Super::Update();

    // カメラにリスナーを設定
    if(auto camera = Scene::Object::Get<Object>("Camera"))
    {
        Set3DSoundListenerPosAndFrontPos_UpVecY(cast(camera->GetTranslate()), cast(-camera->GetVectorAxisZ()));
    }
}

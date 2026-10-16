//===========================================================================
//! @file SceneTutorial.cpp
//! @brief チュートリアルシーン
//===========================================================================
#include "SceneTutorial.h"
#include <System/Component/ComponentModel.h>
#include <Game/Object/Player/Player.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Object/Weapon/Stick.h>
#include <Game/Object/Player/GameUI.h>
#include <Game/Object/Stage.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Object/Tool/AddData.h>
#include <Game/Component/Tutorial/Tutorial.h>
#include <Game/Object/Bottle.h>
#include <System/Utils/IniFileLib.h>
#include <Game/Object/BlackScreen.h>
#include <Game/Object/SkyDome/BlueSkyDome.h>
#include <Game/Object/Sun.h>
#include <Game/Object/FilterBloom.h>
#include <Game/Object/Weapon/AttachWeapon/Barrel.h>
#include <Game/Object/Weapon/AttachWeapon/Box.h>
#include <Game/Object/Weapon/AttachWeapon/TrafficCone.h>
#include <Game/Object/Weapon/AttachWeapon/BarrelWet.h>
#include <Game/Object/Enemy/CloseEnemy/CloseEnemy.h>
#include <Game/Object/FilterVolumetricFog.h>

//! @brief 初期化
//! @return 初期化済み
bool SceneTutorial::Init()
{
    Super::Init();

    // 太陽
    Scene::CreateObjectPtr<Sun>()->SetTranslate(float3{700.0f, 700.0f, -1000.0f});

    volumetric_fog_ = Scene::Object::Get<FilterVolumetricFog>();
    if(auto fog = volumetric_fog_.lock())
    {
        fog->SetHeight(80.0f);
    }

    Scene::Object::Create<BlackScreen>("BlackScreen", true)->SetFadeTime(2.0f)->StartFadeOut(true);
    // 地面
    Scene::Object::Create<Stage>("Stage")->SetTranslate(float3{0.0f, 0.0f, 0.0f});

    // プレイヤー
    auto player = Scene::Object::Create<Player>("Player");
    {
        //// プレイヤーの座標設定
        player->SetTranslate({-600.0f, 100.0f, -311.0f});
        if(auto model = player->GetComponent<ComponentModel>())
        {
            //// プレイヤーモデルの向き
            model->SetRotationAxisXYZ({0.0f, -90.0f, 0.0f});
        }
        if(auto col = player->GetComponent<ComponentCollisionCapsule>())
        {
            col->setGravity(col->gravity() * 3.0f);
        }
    }

    for(int i = 0; i < 2; i++)
    {
        Scene::Object::Create<CloseEnemy>();
    }

    // カメラ
    auto camera = Scene::Object::Create<Camera>("Camera");
    Scene::Object::Create<Stick>();

    Scene::Object::Create<BlueSkyDome>("Sky");
    Scene::Object::Create<GameUI>("GameUI", true);
    Scene::Object::Create<Object>("Tutorial", true)->AddComponent<Tutorial>()->SetPlayer(player)->SetCamera(camera);

    // スポーンするオブジェクトの作成
    Scene::Object::Create<Barrel>();
    Scene::Object::Create<Box>();
    Scene::Object::Create<TrafficCone>();
    Scene::Object::Create<BarrelWet>();

    // 先にキャッシュしておく
    auto cache = Scene::Object::Create<Bottle>();
    Scene::Object::Release(cache);
    return true;
}

//! @brief 更新
void SceneTutorial::Update()
{
    Super::Update();
    if(auto player = Scene::Object::Get<Object>("Player"))
    {
        if(player->GetTranslate().y < -100.0f)
        {
            // シーン再読み込み
            Scene::Change(Scene::GetScene<SceneTutorial>());
        }
    }
}

void SceneTutorial::Exit()
{
    Super::Exit();

    if(auto fog = volumetric_fog_.lock())
    {
        fog->SetHeight();
    }
    volumetric_fog_.reset();
}

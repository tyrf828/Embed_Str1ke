//===========================================================================
//! @file UltimateDash.cpp
//! @brief 必殺技ダッシュ
//===========================================================================
#include "UltimateDash.h"
#include <Game/Component/Player/AnimationController.h>
#include <Game/Component/Player/Move.h>
#include <System/Component/ComponentModel.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Object/Player/CameraAimPoint.h>
#include <Game/Component/ComponentCameraController.h>
#include <Game/Component/Player/UltimateAttack/UltimateRangeAttack.h>
#include <Game/Component/Player/UltimateAttack/UltimateAttack.h>
#include <Game/Object/Player/Player.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Object/Weapon/Stick.h>
#include <Game/Object/Manager/EnemyActionManager.h>
#include <Game/Component/Sound.h>
#include <Game/Object/FilterInkWash.h>
#include <Game/Object/Effect/InkSplash.h>

//! @brief 初期化
void UltimateDash::Init()
{
    Super::Init();
    // カメラをその座標で固定
    if(auto camera_aim = Scene::Object::Get<CameraAimPoint>())
    {
        // プレイヤーのバインドを外す
        camera_aim->SetBindPlayer(false);
        if(auto model = GetOwner()->GetComponent<ComponentModel>())
        {
            // アニメーションフラグを立てる
            model->SetRotationToVector(camera_aim->GetPlaneDir());
        }

        // カメラのマウス移動を無効化
        if(auto camera_control = camera_aim->GetComponent<ComponentCameraController>())
        {
            camera_control->SetUseInput(false);
        }
    }

    if(auto camera = Scene::Object::Get<Camera>())
    {
        // ロックターゲットを解除
        camera->ResetLockTarget();
    }

    if(auto move = GetOwner()->GetComponent<Move>())
    {
        // 移動を無効化
        move->SetEnable(false);
    }
    if(auto anim = GetOwner()->GetComponent<AnimationController>())
    {
        // アニメーションフラグを立てる
        anim->SetFlag(AnimationController::STATE_BIT::ULTIMATE_DASH);
    }
    if(auto player = Scene::Object::Get<Player>())
    {
        if(auto player_col = player->GetComponent<ComponentCollisionCapsule>())
        {
            player_col->SetCollisionGroup(ComponentCollision::CollisionGroup::ETC);
        }
        if(auto model = player->GetComponent<ComponentModel>())
        {
            // 透明処理をするため、フォワードレンダリングを使う
            model->UseForwardRendering(true);
        }
    }
    if(auto stick = Scene::Object::Get<Stick>())
    {
        if(auto stick_model = stick->GetComponent<ComponentModel>())
        {
            // 透明処理をするため、フォワードレンダリングを使う
            stick_model->UseForwardRendering(true);
        }
    }

    if(auto enemy_manager = Scene::Object::Get<EnemyActionManager>())
    {
        // 敵の攻撃を止める
        enemy_manager->StopAttack(true, false);
    }
    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        // 音を再生
        se->PlayNoSame("ultimate start");
    }
    if(!Scene::Object::Get<FilterInkWash>())
    {
        Scene::Object::Create<FilterInkWash>();
        Scene::Object::Create<InkSplash>();
    }

    // プレイヤーの前方ベクトル
    float3 front_vec = GetOwner()->GetComponent<ComponentModel>()->GetPlaneFront();
    ultimate_attack_->SetAttackPos(GetOwner()->GetTranslate() + front_vec * ultimate_attack_->GetRadius());
}

//! @brief 更新
void UltimateDash::Update()
{
    Super::Update();

    auto model       = GetOwner()->GetComponent<ComponentModel>();
    auto stick_model = Scene::Object::Get<Stick>()->GetComponent<ComponentModel>();
    if(!model || !stick_model)
    {
        return;
    }

    // カメラのマウス移動を無効化
    if(auto camera_control = Scene::Object::Get<CameraAimPoint>()->GetComponent<ComponentCameraController>())
    {
        camera_control->AddCameraRotateX(0.0f);
    }

    // プレイヤーモデルの描画優先順位を最低に設定
    if(model)
    {
        Scene::GetCurrentScene()->SetPriority(model, ProcTiming::Draw, ProcPriority::LOWEST);
    }
    alpha_ += GetDeltaTime() * 5.0f;
    alpha_  = min((float1)alpha_, 1.0f);
    // プレイヤーの透明度処理
    {
        auto mesh_num = MV1GetMeshNum(model->GetModel());
        for(int i = 0; i < mesh_num; i++)
        {
            MV1SetMeshOpacityRate(model->GetModel(), i, 1 - alpha_);
        }
    }

    // スティックの透明度処理
    {
        auto mesh_num = MV1GetMeshNum(stick_model->GetModel());
        for(int i = 0; i < mesh_num; i++)
        {
            MV1SetMeshOpacityRate(stick_model->GetModel(), i, MV1GetMeshOpacityRate(model->GetModel(), 0));
        }
    }

    // カメラをその座標で固定
    if(auto camera = Scene::Object::Get<Camera>())
    {
        // アニメーションフラグを立てる
        camera->SetRotationToPosition(Scene::Object::Get<Player>()->GetTranslate());
    }
    dash_time_ += GetDeltaTime();
    if(dash_time_ < DASH_TIME_)
    {
        GetOwner()->AddTranslate(model->GetPlaneFront() * 2.0f, true);
    }
    else
    {
        // 範囲攻撃に移行する
        ultimate_attack_->SetAction(std::make_shared<UltimateRangeAttack>());
    }
}

//! @brief 描画
void UltimateDash::LateDraw()
{
    Super::LateDraw();
}

//! @brief 終了
void UltimateDash::Exit()
{
    Super::Exit();
    if(auto anim = GetOwner()->GetComponent<AnimationController>())
    {
        // アニメーションフラグをリセット
        anim->ResetFlag(AnimationController::STATE_BIT::ULTIMATE_DASH);
    }
    if(auto model = Scene::Object::Get<Player>()->GetComponent<ComponentModel>())
    {
        model->SetScaleAxisXYZ(float3{FLT_EPSILON, FLT_EPSILON, FLT_EPSILON});
    }
}

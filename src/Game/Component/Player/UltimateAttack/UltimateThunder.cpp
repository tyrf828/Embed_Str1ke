//===========================================================================
//! @file UltimateThunder.cpp
//! @brief 必殺技の雷攻撃
//===========================================================================
#include "UltimateThunder.h"
#include <Game/Component/Player/UltimateAttack/UltimateAttack.h>
#include <Game/Object/Effect/FinishThunderEffect.h>
#include <System/Component/ComponentModel.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Object/Effect/UltimateAttackEffect.h>
#include <Game/Object/Player/CameraAimPoint.h>
#include <Game/Component/Player/AnimationController.h>
#include <Game/Component/ComponentCameraController.h>
#include <Game/Component/Player/Move.h>
#include <Game/Object/Player/Player.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/Weapon/AttachWeaponParts.h>
#include <Game/Component/Weapon/ReactiveForce.h>
#include <Game/Object/Weapon/Stick.h>
#include <Game/Component/Weapon/AttachWeapon.h>
#include <Game/Component/Weapon/AttachWeaponRemain.h>
#include <Game/Object/Effect/SlashEffect.h>
#include <Game/Object/Manager/EnemyActionManager.h>
#include <System/Lib.h>
#include <Game/Component/Sound.h>
#include <Game/Object/Effect/FloorBreakEffect.h>
#include <Game/Object/FilterInkWash.h>

//! @brief 初期化
void UltimateThunder::Init()
{
    Super::Init();

    if(auto player = Scene::Object::Get<Player>())
    {
        Scene::Object::Create<FinishThunderEffect>("FinishThunderEffect")->SetTranslate(player->GetTranslate());

        auto model = player->GetComponent<ComponentModel>();
        if(model)
        {
            model->SetScaleAxisXYZ(float3{0.1f, 0.1f, 0.1f});

            float3 model_vec = normalize(model->GetVectorAxisZ());
            // 地面が割れるエフェクトを生成
            Scene::Object::Create<FloorBreakEffect>("FloorBreakEffect")->SetTranslate(player->GetTranslate())->SetRotationToVector(model_vec);
        }
    }
    if(auto anim = GetOwner()->GetComponent<AnimationController>())
    {
        // アニメーションフラグをリセット
        anim->SetFlag(AnimationController::STATE_BIT::ULTIMATE_FINISH);
    }

    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        // 音を再生
        se->PlayNoSame("ultimate thunder");
    }

    if(auto ink = Scene::Object::Get<FilterInkWash>())
    {
        Scene::Object::Release(ink);
    }

    auto slash  = Scene::Object::Create<SlashEffect>("SlashEffect");
    slash_zoom_ = slash->GetZoom();
}

//! @brief 更新
void UltimateThunder::Update()
{
    Super::Update();

    auto model       = GetOwner()->GetComponent<ComponentModel>();
    auto aim_point   = Scene::Object::Get<CameraAimPoint>();
    auto anim        = GetOwner()->GetComponent<AnimationController>();
    auto stick_model = Scene::Object::Get<Stick>()->GetComponent<ComponentModel>();
    if(!model || !aim_point || !anim || !stick_model)
    {
        return;
    }

    // 雷を表現するため一瞬画面全体の白いを描画
    thunder_alpha_ -= (int)(255 * GetDeltaTime());
    thunder_alpha_  = max(0, (int1)(thunder_alpha_));

    // プレイヤーモデルの描画優先順位を設定
    if(model)
    {
        Scene::GetCurrentScene()->SetPriority(model, ProcTiming::Draw, ProcPriority::LOW);
    }

    if(Scene::Object::Get<FinishThunderEffect>())
    {
        for(auto& obj : Scene::Object::GetArray<UltimateAttackEffect>())
        {
            Scene::Object::Release(obj);
        }
        Scene::Object::Release(Scene::Object::Get<UltimateAttackCollision>());
        if(auto cam = Scene::Object::Get<Camera>())
        {
            cam->ShakeCamera(0.1f, 0.5f);
        }
    }

    alpha_ += GetDeltaTime() * 2.0f;
    if(alpha_ >= 1.0f)
    {
        alpha_ = 1.0f;

        // プレイヤーの透明度を完全不透明に設定
        {
            auto mesh_num = MV1GetMeshNum(model->GetModel());
            for(int i = 0; i < mesh_num; i++)
            {
                MV1SetMeshOpacityRate(model->GetModel(), i, 1.0f);
            }
        }
        // スティックの透明度を完全不透明に設定
        {
            auto mesh_num = MV1GetMeshNum(stick_model->GetModel());
            for(int i = 0; i < mesh_num; i++)
            {
                MV1SetMeshOpacityRate(stick_model->GetModel(), i, 1.0f);
            }
        }

        // フォワードレンダリングを無効化
        model->UseForwardRendering(false);
        stick_model->UseForwardRendering(false);
    }
    else
    {
        // プレイヤーの透明度処理
        {
            auto mesh_num = MV1GetMeshNum(model->GetModel());
            for(int i = 0; i < mesh_num; i++)
            {
                MV1SetMeshOpacityRate(model->GetModel(), i, alpha_);
            }
        }
        // スティックの透明度処理
        {
            auto mesh_num = MV1GetMeshNum(stick_model->GetModel());
            for(int i = 0; i < mesh_num; i++)
            {
                MV1SetMeshOpacityRate(stick_model->GetModel(), i, alpha_);
            }
        }
    }

    if(anim->GetFlag(AnimationController::STATE_BIT::ULTIMATE_FINISH))
    {
        if(auto parts = Scene::Object::Get<Stick>()->GetComponent<AttachWeaponParts>())
        {
            for(auto p : parts->GetParts())
            {
                if(!p->GetComponent<ReactiveForce>())
                {
                    // ランダムの方向に飛ばす
                    float rand_y = GetRandomF(360.0f);

                    p->AddComponent<ReactiveForce>()->SetDirection(
                        float3(0.0f, 1.0f, 0.0f) + float3(0.5f * cosf(rand_y * DegToRad), 0.0f, 0.5f * sinf(rand_y * DegToRad)), 1.0f, 40.0f);
                }

                if(auto remain = p->GetComponent<AttachWeaponRemain>())
                {
                    remain->SetRemain(0);
                }
            }
        }

        // プレイヤーにバインドする
        aim_point->SetBindPlayer();
    }
    else
    {
        aim_point->SetRotationToVectorWithLimit(model->GetPlaneDir(), 5.0f);
    }

    if(anim->GetNowPlaying((int)AnimationController::STATE::IDLE))
    {
        if(model->GetAnimationTime() > 2.0f)
        {
            // カメラをプレイヤの位置に戻す
            ultimate_attack_->RemoveThisComponent();
            aim_point->SetBindPlayer();
        }
    }
    if(auto slash = Scene::Object::Get<SlashEffect>())
    {
        // スラッシュエフェクトのズーム処理
        slash_time_       += GetDeltaTime() * 0.4f;
        float smooth_time  = smoothstep((float1)0.0f, (float1)1.0f, slash_time_);
        float zoom_        = lerp((float1)slash_zoom_, (float1)1.0f, smooth_time);
        slash->SetZoom(zoom_);
    }
}

//! @brief 描画
void UltimateThunder::LateDraw()
{
    Super::LateDraw();

    // 雷を表現するため一瞬画面全体の白いを描画
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, thunder_alpha_);
    DrawBoxAA(0, 0, (float)WINDOW_W, (float)WINDOW_H, GetColor(214, 214, 214), true);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, -1);
}

//! @brief 終了
void UltimateThunder::Exit()
{
    Super::Exit();

    if(auto camera_aim = Scene::Object::Get<CameraAimPoint>())
    {
        // 入力使用可能
        if(auto cam_control = camera_aim->GetComponent<ComponentCameraController>())
        {
            cam_control->SetUseInput(true);
        }
    }

    // 移動を有効化
    if(auto move = GetOwner()->GetComponent<Move>())
    {
        move->SetEnable(true);
    }
    if(auto player = Scene::Object::Get<Player>())
    {
        if(auto player_col = player->GetComponent<ComponentCollisionCapsule>())
        {
            player_col->SetCollisionGroup(ComponentCollision::CollisionGroup::PLAYER);
        }
        player->SetActionEnable(true);
    }

    if(auto enemy_manager = Scene::Object::Get<EnemyActionManager>())
    {
        enemy_manager->StopAttack(false);
    }
}

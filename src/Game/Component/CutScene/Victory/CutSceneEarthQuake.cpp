//===========================================================================
//! @file CutSceneEarthQuake.cpp
//! @brief 地震のカットシーン
//===========================================================================
#include "CutSceneEarthQuake.h"
#include "System/Component/ComponentModel.h"
#include <Game/Component/Sound.h>
#include <Game/Component/CutScene/Animation/CutScenePlayerAnimationController.h>
#include <Game/Object/Crystal.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Component/CutScene/Victory/CutSceneStageFade.h>
#include <System/Lib.h>
#include <System/Component/ComponentAttachModel.h>

//! @brief 初期化
//! @param info カットシーンの保護情報
void CutSceneEarthQuake::Init(const ProtectInfo& info)
{
    CutSceneBase::Init(info);

    if(auto camera = info_.camera_.lock())
    {
        // カメラの設定
        camera->SetTranslate(float3{15.1f, 18.6f, -17.1f});
        camera->SetRotationToPosition(info_.player_.lock()->GetTranslate() + float3{0.0f, 25.0f, 0.0f}, float3{0.0f, 1.0f, 0.0f}, true);

        if(auto cmp_cam = camera->GetComponent<ComponentCamera>())
        {
            initial_perspective_ = cmp_cam->GetPerspective();
            perspective_time_    = 0.0f;
        }
    }
    if(auto crystal = Scene::Object::Get<Crystal>("Crystal"))
    {
        // プレイヤーの手にアタッチ
        auto attach = crystal->AddComponent<ComponentAttachModel>();
        attach->SetAttachObject(info_.player_.lock(), "mixamorig:RightHand");
        attach->SetAttachRotate(float3{0.0f, 0.0f, -90.0f});
        attach->SetAttachOffset(float3{-2.0f, 9.0f, -5.0f});
        if(auto model = crystal->GetComponent<ComponentModel>())
        {
            model->SetScaleAxisXYZ(float3{0.5f, 0.5f, 0.5f});
        }
    }
}

//! @brief 更新
void CutSceneEarthQuake::Update()
{
    Super::Update();
    auto player = info_.player_.lock();
    auto camera = info_.camera_.lock();

    if(!player || !camera)
    {
        return;
    }

    if(auto cmp_cam = camera->GetComponent<ComponentCamera>())
    {
        // 視野角を変更
        if(perspective_time_ < PERSPECTIVE_TRANSITION_TIME_)
        {
            perspective_time_         += GetDeltaTime();
            float t                    = perspective_time_ / PERSPECTIVE_TRANSITION_TIME_;
            t                          = Clamp01(t);
            float current_perspective  = initial_perspective_ + (TARGET_PERSPECTIVE_ - initial_perspective_) * t;
            cmp_cam->SetPerspective(current_perspective);
        }
        else
        {
            cmp_cam->SetPerspective(TARGET_PERSPECTIVE_);
        }
    }

    if(auto anim = player->GetComponent<CutScenePlayerAnimationController>())
    {
        // アニメーションの特定の時間でカメラを揺らす
        if(anim->IsAnimationTimeInRange(11.7f))
        {
            if(camera && !is_camera_shaked_)
            {
                original_camera_pos_ = camera->GetTranslate();
                shake_time_          = 0.5f;
                is_camera_shaked_    = true;
            }
        }

        // カメラの揺れ処理
        if(shake_time_ > 0.0f)
        {
            shake_time_ -= GetDeltaTime();

            float rand_x        = GetRandomF(-POWER_, POWER_);
            float rand_y        = GetRandomF(-POWER_, POWER_);
            float rand_z        = GetRandomF(-POWER_, POWER_);
            float3 shake_offset = {rand_x, rand_y, rand_z};

            camera->SetTranslate(original_camera_pos_ + shake_offset);
        }
        else if(is_camera_shaked_ && shake_time_ <= 0.0f)
        {
            // 揺れ終了後、次のカットシーンへ遷移
            camera->SetTranslate(original_camera_pos_);
            ChangeCutScene<CutSceneStageFade>();
        }
    }
}

//! @brief GUI表示
void CutSceneEarthQuake::GUI()
{
    Super::GUI();
    ImGui::Begin(info_.owner_->GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("CutSceneEarthQuake");
    }
    ImGui::End();
}

//! @brief 終了処理
void CutSceneEarthQuake::Exit()
{
    Super::Exit();
    if(auto player = info_.player_.lock())
    {
        if(auto anim = player->GetComponent<CutScenePlayerAnimationController>())
        {
            // アニメーションをリセット
            anim->ResetFlag(CutScenePlayerAnimationController::STATE_BIT::PICK_UP);
        }
    }
}

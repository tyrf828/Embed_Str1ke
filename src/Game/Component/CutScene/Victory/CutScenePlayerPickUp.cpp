//===========================================================================
//! @file CutScenePlayerPickUp.cpp
//! @brief プレイヤーがアイテムを拾うカットシーン
//===========================================================================
#include "CutScenePlayerPickUp.h"
#include "System/Component/ComponentModel.h"
#include <Game/Object/BlackScreen.h>
#include <Game/Component/Sound.h>
#include <Game/Component/CutScene/Animation/CutScenePlayerAnimationController.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <Game/Object/Crystal.h>
#include <Game/Component/CutScene/Victory/CutSceneEarthQuake.h>

//! @brief 初期化
//! @param info カットシーンの保護情報
void CutScenePlayerPickUp::Init(const ProtectInfo& info)
{
    CutSceneBase::Init(info);

    // フェードアウト開始
    Scene::Object::Create<BlackScreen>("BlackScreen")->SetFadeTime(2.0f)->StartFadeOut(true);

    if(auto camera = info_.camera_.lock())
    {
        // カメラの設定
        camera->SetTranslate(float3{12.3f, 5.0f, 3.7f});
        camera->SetRotationAxisXYZ(float3{-17.0f, 66.0f, 0.0f});
        if(auto cmp_cam = camera->GetComponent<ComponentCamera>())
        {
            // 視野角を設定
            cmp_cam->SetPerspective(INIT_PERSPECTIVE_);
        }
    }

    // プレイヤー
    if(auto player = info_.player_.lock())
    {
        if(auto model = player->GetComponent<ComponentModel>())
        {
            // モデルのXZ軸の原点固定を無効化
            model->SetOnPlaceXZ(false);
        }
        if(auto anim = player->GetComponent<CutScenePlayerAnimationController>())
        {
            // アニメーションを設定
            anim->SetFlag(CutScenePlayerAnimationController::STATE_BIT::PICK_UP);
            anim->SetTimeScale(0.1f);
        }
        // プレイヤーの位置を設定
        player->SetTranslate(float3{0.0f, 0.0f, 5.0f});
    }

    if(auto crystal = Scene::Object::Get<Crystal>("Crystal"))
    {
        // クリスタルの設定
        crystal->SetTranslate(float3{0.0f, 0.5f, -1.0f});
        crystal->SetRotationAxisXYZ(float3{0.0f, 60.0f, -73.5f});
        if(auto se = crystal->GetComponent<Sound>())
        {
            // 音を再生
            se->PlayNoSame("drop");
        }
    }
}

//! @brief 更新
void CutScenePlayerPickUp::Update()
{
    Super::Update();
    auto player = info_.player_.lock();
    auto camera = info_.camera_.lock();

    if(!player || !camera)
    {
        return;
    }

    scene_time_ += GetDeltaTime();

    if(auto cmp_cam = camera->GetComponent<ComponentCamera>())
    {
        // 視野角を変更
        if(cmp_cam->GetPerspective() < TARGET_PERSPECTIVE_)
        {
            float diff        = TARGET_PERSPECTIVE_ - INIT_PERSPECTIVE_;
            float perspective = INIT_PERSPECTIVE_ + diff * scene_time_ * 0.2f;
            camera->GetComponent<ComponentCamera>()->SetPerspective(perspective);
        }
    }

    // 一定時間経過後、アニメーション速度を戻す
    if(scene_time_ > CHANGE_NEXT_TIME_ * 0.5f)
    {
        if(auto anim = player->GetComponent<CutScenePlayerAnimationController>())
        {
            anim->SetTimeScale(1.0f);
        }
    }

    // 次のカットシーンへ遷移
    if(scene_time_ > CHANGE_NEXT_TIME_)
    {
        ChangeCutScene<CutSceneEarthQuake>();
    }
}

//! @brief GUI表示
void CutScenePlayerPickUp::GUI()
{
    Super::GUI();
    ImGui::Begin(info_.owner_->GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("CutScenePlayerPickUp");
    }
    ImGui::End();
}

//! @brief 終了処理
void CutScenePlayerPickUp::Exit()
{
    Super::Exit();
    if(auto player = info_.player_.lock())
    {
        if(auto model = player->GetComponent<ComponentModel>())
        {
            // モデルのXZ軸を原点に固定
            model->SetOnPlaceXZ(true);
        }
    }
}

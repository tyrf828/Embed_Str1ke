//===========================================================================
//! @file CutScenePlayerWalkIn.cpp
//! @brief プレイヤーが入場するカットシーン
//===========================================================================
#include "CutScenePlayerWalkIn.h"
#include "System/Component/ComponentModel.h"
#include <Game/Component/CutScene/Animation/CutScenePlayerAnimationController.h>
#include <Game/Component/CutScene/EnterBoss/CutSceneBossLook.h>
#include <Game/Object/BlackScreen.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
//! @param info カットシーンの保護情報
void CutScenePlayerWalkIn::Init(const ProtectInfo& info)
{
    CutSceneBase::Init(info);

    // フェードアウト開始
    Scene::Object::Create<BlackScreen>("BlackScreen")->SetFadeTime(2.0f)->StartFadeOut(true);

    if(auto camera = info_.camera_.lock())
    {
        // カメラの設定
        camera->SetTranslate(float3{-821.0f, 104.0f, 4.5f});
        camera->SetRotationAxisXYZ(float3{-25.0f, -160.0f, 0.0f});
    }

    // プレイヤー
    if(auto player = info_.player_.lock())
    {
        if(auto anim = player->GetComponent<CutScenePlayerAnimationController>())
        {
            // 歩くアニメーションを設定
            anim->SetFlag(CutScenePlayerAnimationController::STATE_BIT::WALK);
        }

        // プレイヤーの設定
        player->SetTranslate({-839.0f, 97.0f, -56.0f});
        player->SetRotationAxisXYZ(float3{0.0f, -152.0f, 0.0f});
    }
    if(auto boss = info_.boss_.lock())
    {
        // ボスの位置を設定
        boss->SetTranslate(float3{-785.0f, 150.0f, 100.0f});
    }
}

//! @brief 更新
void CutScenePlayerWalkIn::Update()
{
    Super::Update();
    auto player = info_.player_.lock();
    auto camera = info_.camera_.lock();
    if(!player || !camera)
    {
        return;
    }
    scene_time_ += GetDeltaTime();
    // 一定時間経過したら次のカットシーンへ
    if(scene_time_ >= CHANGE_NEXT_TIME_)
    {
        ChangeCutScene<CutSceneBossLook>();
        return;
    }
    // 移動速度
    const float WALK_SPEED = 0.1f;
    // プレイヤーを移動
    player->AddTranslate(-player->GetVectorAxisZ() * WALK_SPEED);
    // カメラを移動
    camera->AddTranslate(-player->GetVectorAxisZ() * WALK_SPEED * 0.5f);

    if(auto se = player->GetComponent<Sound>())
    {
        // 歩く音を再生
        se->PlayNoSame("walk");
    }
}

//! @brief GUI表示
void CutScenePlayerWalkIn::GUI()
{
    Super::GUI();
    ImGui::Begin(info_.owner_->GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("CutScenePlayerWalkIn");
    }
    ImGui::End();
}

//! @brief 終了処理
void CutScenePlayerWalkIn::Exit()
{
    Super::Exit();
    if(auto player = info_.player_.lock())
    {
        if(auto se = player->GetComponent<Sound>())
        {
            // 歩く音を停止
            se->Stop("walk");
        }
    }
}

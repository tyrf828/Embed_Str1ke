//===========================================================================
//! @file CutSceneFight.cpp
//! @brief プレイヤーとボスの対峙カットシーン
//===========================================================================
#include "CutSceneFight.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/CutScene/Animation/CutScenePlayerAnimationController.h>
#include <Game/Component/CutScene/Animation/CutSceneBossAnimationController.h>
#include <Game/Object/BlackScreen.h>
#include <Game/Scene/SceneBoss.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
//! @param info カットシーンの保護情報
void CutSceneFight::Init(const ProtectInfo& info)
{
    CutSceneBase::Init(info);
    if(auto camera = info_.camera_.lock())
    {
        // カメラの設定
        camera->SetTranslate(float3{-804.5f, 110.5f, 23.0f});
        camera->SetRotationAxisXYZ(float3{-45.0f, 90.0f, 0.0f});
    }
    if(auto player = info_.player_.lock())
    {
        // プレイヤーの設定
        player->SetTranslate(float3{-772.0f, 97.0f, 10.4f});
        player->SetRotationAxisXYZ(float3{0.0f, 180.0f, 0.0f});
        if(auto anim_control = player->GetComponent<CutScenePlayerAnimationController>())
        {
            // 攻撃アニメーションを設定
            anim_control->SetFlag(CutScenePlayerAnimationController::STATE_BIT::ATTACK);
        }
        if(auto model = player->GetComponent<ComponentModel>())
        {
            // タイムスケールを変更
            model->SetTimeScale(0.7f);
        }
    }
    if(auto boss = info_.boss_.lock())
    {
        // ボスの位置を設定
        boss->SetTranslate(float3{-774.0f, 137.0f, 62.4f});
        if(auto model = boss->GetComponent<ComponentModel>())
        {
            // アニメーションを再開し、タイムスケールを変更
            model->PlayPause(false);
            model->SetTimeScale(0.3f);
        }
    }
    // フェードイン開始
    Scene::Object::Create<BlackScreen>("BlackScreen")->StartFadeIn();
}

//! @brief 更新
void CutSceneFight::Update()
{
    Super::Update();
    auto player = info_.player_.lock();
    auto boss   = info_.boss_.lock();
    if(!player || !boss)
    {
        return;
    }
    auto player_anim = player->GetComponent<CutScenePlayerAnimationController>();
    auto boss_anim   = boss->GetComponent<CutSceneBossAnimationController>();
    if(!player_anim || !boss_anim)
    {
        return;
    }
    if(auto screen = Scene::Object::Get<BlackScreen>())
    {
        // フェード完了後
        if(screen->IsFadeComplete())
        {
            if(auto se = player->GetComponent<Sound>())
            {
                // 攻撃音を再生
                se->PlayOnce("attack");
            }
            scene_time_ += GetDeltaTime();
            // 一定時間経過したらボスシーンへ遷移
            if(scene_time_ >= CHANGE_NEXT_TIME_)
            {
                Scene::Change(Scene::GetScene<SceneBoss>());
            }
            return;
        }
    }

    // プレイヤーのアニメーション一時停止
    if(player_anim->IsAnimationTimeInRange(1.58f))
    {
        if(auto model = player->GetComponent<ComponentModel>())
        {
            model->PlayPause();
        }
    }

    // ボスのアニメーション一時停止
    if(boss_anim->IsAnimationTimeInRange(1.27f))
    {
        if(auto model = boss->GetComponent<ComponentModel>())
        {
            model->PlayPause();
        }
    }
    float distance = length(player->GetTranslate() - boss->GetTranslate());
    // 距離が離れている場合、ボスをプレイヤーに近づける
    if(distance > 30.0f)
    {
        float3 to_player    = player->GetTranslate() - boss->GetTranslate();
        const float OFFSET  = 10;   // 少し上にオフセットをかける
        to_player.y        += OFFSET;
        to_player           = normalize(to_player);
        boss->AddTranslate(to_player * 0.4f);
    }
}

//! @brief GUI表示
void CutSceneFight::GUI()
{
    Super::GUI();
    ImGui::Begin(info_.owner_->GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("CutSceneFight");
    }
    ImGui::End();
}

//! @brief 終了処理
void CutSceneFight::Exit()
{
    Super::Exit();
}

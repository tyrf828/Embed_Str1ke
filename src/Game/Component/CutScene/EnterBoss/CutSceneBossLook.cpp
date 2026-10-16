//===========================================================================
//! @file CutSceneBossLook.cpp
//! @brief ボスが遠くを見るカットシーン
//===========================================================================
#include "CutSceneBossLook.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/CutScene/EnterBoss/CutSceneBossJump.h>
#include <Game/Component/CutScene/Animation/CutSceneBossAnimationController.h>
#include <Game/Component/Sound.h>
#include <Game/Component/CutScene/Animation/CutScenePlayerAnimationController.h>

//! @brief 初期化
//! @param info カットシーンの保護情報
void CutSceneBossLook::Init(const ProtectInfo& info)
{
    CutSceneBase::Init(info);

    if(auto camera = info_.camera_.lock())
    {
        // カメラの設定
        camera->SetTranslate(float3{-764.0f, 169.0f, 137.5f});
        camera->SetRotationAxisXYZ(float3{0.0f, -152.0f, 0.0f});
    }
    if(auto boss = info_.boss_.lock())
    {
        // ボスの設定
        boss->SetTranslate(float3{-774.0f, 174.5f, 153.0f});
        boss->SetRotationAxisXYZ(float3{-24.0f, 0.0f, 0.0f});
        if(auto col = boss->GetComponent<ComponentCollisionCapsule>())
        {
            // 重力を無効化
            col->UseGravity(false);
        }
        if(auto anim_control = boss->GetComponent<CutSceneBossAnimationController>())
        {
            // 座るアニメーションを再生
            anim_control->SetFlag(CutSceneBossAnimationController::STATE_BIT::SIT);
        }
    }
    if(auto player = info_.player_.lock())
    {
        if(auto se = player->GetComponent<Sound>())
        {
            // 歩く音を再生
            se->PlayNoSame("walk");
        }
    }
}

//! @brief 更新
void CutSceneBossLook::Update()
{
    Super::Update();
    auto player = info_.player_.lock();
    auto boss   = info_.boss_.lock();
    auto camera = info_.camera_.lock();
    if(!boss || !camera)
    {
        return;
    }
    scene_time_ += GetDeltaTime();
    // 一定時間経過したら次のカットシーンへ
    if(scene_time_ >= CHANGE_NEXT_TIME_)
    {
        ChangeCutScene<CutSceneBossJump>();
        return;
    }
    const float WALK_SPEED = 0.1f;
    // プレイヤーを移動
    player->AddTranslate(-player->GetVectorAxisZ() * WALK_SPEED);
    // カメラを移動
    camera->AddTranslate(boss->GetVectorAxisZ() * WALK_SPEED * 0.5f);
}

//! @brief GUI表示
void CutSceneBossLook::GUI()
{
    Super::GUI();
    ImGui::Begin(info_.owner_->GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("CutSceneBossLook");
    }
    ImGui::End();
}

//! @brief 終了処理
void CutSceneBossLook::Exit()
{
    Super::Exit();
    if(auto player = info_.player_.lock())
    {
        if(auto se = player->GetComponent<Sound>())
        {
            // 歩く音を停止
            se->Stop("walk");
        }
        if(auto anim = player->GetComponent<CutScenePlayerAnimationController>())
        {
            // 歩くアニメーションをリセット
            anim->ResetFlag(CutScenePlayerAnimationController::STATE_BIT::WALK);
        }
    }
}

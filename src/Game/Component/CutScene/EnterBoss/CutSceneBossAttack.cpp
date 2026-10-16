//===========================================================================
//! @file CutSceneBossAttack.cpp
//! @brief ボスが攻撃するカットシーン
//===========================================================================
#include "CutSceneBossAttack.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentCamera.h>
#include <Game/Component/CutScene/Animation/CutScenePlayerAnimationController.h>
#include <Game/Component/CutScene/Animation/CutSceneBossAnimationController.h>
#include <Game/Component/CutScene/EnterBoss/CutSceneFight.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
//! @param info カットシーンの保護情報
void CutSceneBossAttack::Init(const ProtectInfo& info)
{
    CutSceneBase::Init(info);

    // ボスの初期化
    if(auto boss = info_.boss_.lock())
    {
        // ジャンプアニメーションを再生
        if(auto anim_control = boss->GetComponent<CutSceneBossAnimationController>())
        {
            anim_control->PlayOnce((int)CutSceneBossAnimationController::STATE::JUMP);
        }
        // ボスの位置を設定
        boss->SetTranslate(BOSS_START_POS_);
        // Y軸固定を有効化
        if(auto model = boss->GetComponent<ComponentModel>())
        {
            model->SetOnPlaceY(true);
        }
        // 攻撃音を再生
        if(auto se = boss->GetComponent<Sound>())
        {
            se->PlayNoSame("attack");
        }
    }
    // カメラの初期化
    if(auto camera = info_.camera_.lock())
    {
        camera->SetTranslate(float3{-774.0f, 117.5f, -3.5f});
        camera->SetRotationAxisXYZ(float3{-52.0f, 0.0f, 0.0f});
        if(auto cmp_camera = camera->GetComponent<ComponentCamera>())
        {
            cmp_camera->SetPerspective(CAMERA_FOV_);
        }
    }
    // プレイヤーの初期化
    if(auto player = info_.player_.lock())
    {
        player->SetTranslate(float3{-881.0f, 105.0f, -18.0f});
    }
}

//! @brief 更新
void CutSceneBossAttack::Update()
{
    Super::Update();
    auto boss   = info_.boss_.lock();
    auto camera = info_.camera_.lock();
    if(!boss || !camera)
    {
        return;
    }

    auto boss_anim = boss->GetComponent<CutSceneBossAnimationController>();
    if(!boss_anim)
    {
        return;
    }
    float gravity  = 9.8f;   // 重力加速度
    scene_time_   += GetDeltaTime();

    const float JUMP_FORCE = 60.0f;   // ジャンプの力

    float force = JUMP_FORCE * scene_time_ - (0.5f * gravity * scene_time_ * scene_time_);   // ジャンプの力を計算

    // ジャンプアニメーション再生中
    if(boss_anim->IsPlaying() && boss_anim->GetNowPlaying((int)CutSceneBossAnimationController::STATE::JUMP))
    {
        // カメラを回転
        camera->AddRotationAxisXYZ(float3{-scene_time_ * 0.5f, 0.0f, 0.0f});
        // ボスをジャンプさせる
        boss->SetTranslate(BOSS_START_POS_ + float3{0.0f, force, 0.0f});
        return;
    }
    // 攻撃アニメーションを設定
    boss_anim->SetFlag(CutSceneBossAnimationController::STATE_BIT::ATTACK);

    // ボスをカメラに向かって移動
    float3 to_camera = camera->GetTranslate() - boss->GetTranslate();
    to_camera        = normalize(to_camera);
    boss->AddTranslate(to_camera);

    // カメラの視野角を変更
    if(auto cmp_camera = camera->GetComponent<ComponentCamera>())
    {
        camera_time_ += GetDeltaTime();
        float fov     = CAMERA_FOV_ + (CAMERA_FINAL_FOV_ - CAMERA_FOV_) * camera_time_ / 1.0f;
        fov           = std::min(fov, CAMERA_FINAL_FOV_);
        cmp_camera->SetPerspective(fov);
    }
    // アニメーション終了時
    if(boss_anim->IsAnimationTimeInRange(1.1f))
    {
        if(auto model = boss->GetComponent<ComponentModel>())
        {
            // アニメーションを一時停止
            model->PlayPause(true);
        }
        // 次のカットシーンに遷移
        ChangeCutScene<CutSceneFight>();
        return;
    }
}

//! @brief GUI表示
void CutSceneBossAttack::GUI()
{
    Super::GUI();
    ImGui::Begin(info_.owner_->GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("CutSceneBossAttack");
    }
    ImGui::End();
}

//! @brief 終了処理
void CutSceneBossAttack::Exit()
{
    Super::Exit();
}

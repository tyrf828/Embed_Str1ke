//===========================================================================
//! @file CutSceneBossJump.cpp
//! @brief ボスがジャンプするカットシーン
//===========================================================================
#include "CutSceneBossJump.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/CutScene/Animation/CutSceneBossAnimationController.h>
#include <Game/Component/CutScene/EnterBoss/CutSceneBossAttack.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
//! @param info カットシーンの保護情報
void CutSceneBossJump::Init(const ProtectInfo& info)
{
    CutSceneBase::Init(info);

    if(auto boss = info_.boss_.lock())
    {
        if(auto anim_control = boss->GetComponent<CutSceneBossAnimationController>())
        {
            // 立ち上がるアニメーションを再生
            anim_control->SetFlag(CutSceneBossAnimationController::STATE_BIT::STAND_UP);
        }
        // カメラの移動方向を設定
        camera_move_dir_ = boss->GetVectorAxisZ();
        if(auto se = boss->GetComponent<Sound>())
        {
            // 立ち上がる音を再生
            se->PlayNoSame("stand up");
        }
    }
}

//! @brief 更新
void CutSceneBossJump::Update()
{
    Super::Update();
    auto player = info_.player_.lock();
    auto boss   = info_.boss_.lock();
    auto camera = info_.camera_.lock();
    if(!player || !boss || !camera)
    {
        return;
    }

    if(auto anim_control = boss->GetComponent<CutSceneBossAnimationController>())
    {
        // ボスを回転させる
        boss->SetRotationToVectorWithLimit(-boss->GetVectorAxisZ(), 2.0f);
        // アニメーション時間が6.0秒を超えた場合
        if(anim_control->IsAnimationTimeInRange(6.0f))
        {
            // 立ち上がるアニメーションをリセット
            anim_control->ResetFlag(CutSceneBossAnimationController::STATE_BIT::STAND_UP);
            // ジャンプアニメーションを再生
            anim_control->SetFlag(CutSceneBossAnimationController::STATE_BIT::JUMP);
        }
        // ジャンプアニメーション再生中
        if(anim_control->GetFlag(CutSceneBossAnimationController::STATE_BIT::JUMP))
        {
            // ボスを上げる
            boss->AddTranslate(float3{0.0f, 0.3f, 0.0f});
        }
        // アニメーション終了時
        if(!anim_control->IsPlaying())
        {
            // 次のカットシーンに遷移
            ChangeCutScene<CutSceneBossAttack>();
            return;
        }
    }

    const float WALK_SPEED = 0.1f;
    // プレイヤーを前進させる
    player->AddTranslate(-player->GetVectorAxisZ() * WALK_SPEED);

    scene_time_ += GetDeltaTime();
    // カメラ停止時間を超えた場合
    if(scene_time_ >= CAMERA_STOP_TIME)
    {
        return;
    }

    // カメラを移動させる
    camera->AddTranslate(camera_move_dir_ * WALK_SPEED * 0.5f);
}

//! @brief GUI表示
void CutSceneBossJump::GUI()
{
    Super::GUI();
    ImGui::Begin(info_.owner_->GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("CutSceneBossJump");
    }
    ImGui::End();
}

//! @brief 終了処理
void CutSceneBossJump::Exit()
{
    Super::Exit();

    if(auto boss = info_.boss_.lock())
    {
        if(auto trans = boss->GetComponent<ComponentTransform>())
        {
            // ボスの回転をリセット
            trans->SetRotationAxisXYZ(float3{0.0f, 0.0f, 0.0f});
        }
    }
}

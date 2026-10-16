//===========================================================================
//! @file CutSceneStageFade.cpp
//! @brief ステージが消えるカットシーン
//===========================================================================
#include "CutSceneStageFade.h"
#include <Game/Component/DialogueVoice.h>
#include "System/Component/ComponentModel.h"
#include <Game/Component/Sound.h>
#include <Game/Component/CutScene/Animation/CutScenePlayerAnimationController.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <Game/Object/Crystal.h>
#include <Game/Component/SphericalMaskTransition.h>
#include <Game/Object/Stage.h>
#include <Game/Object/BossStage.h>
#include <Game/Component/UI/AutoDrawText.h>
#include <Game/Component/CutScene/Victory/CutSceneRunEscape.h>
#include <Game/Object/BlackScreen.h>
#include <Game/Object/SkyDome/BlueSkyDome.h>
#include <Game/Object/SkyDome/BossSkyDome.h>

//! @brief 初期化
//! @param info カットシーンの保護情報
void CutSceneStageFade::Init(const ProtectInfo& info)
{
    CutSceneBase::Init(info);

    // ゲームステージのフェード処理
    if(auto game_stage = Scene::Object::Get<Stage>("GameStage"))
    {
        game_stage->SetStatus(Object::StatusBit::NoDraw, false);
        auto spherical = game_stage->AddComponent<SphericalMaskTransition>();
        spherical->SetMaskInverse(true);
        spherical->SetMaskRadius(700.0f);
        spherical->SetDestroyLimit(0.0f, false);
        spherical->SetSpeed(-3);
    }

    // スカイドームのフェード処理
    if(auto sky = Scene::Object::Get<BlueSkyDome>())
    {
        sky->SetStatus(Object::StatusBit::NoDraw, false);
        auto spherical = sky->AddComponent<SphericalMaskTransition>();
        spherical->SetMaskInverse(true);
        spherical->SetDestroyLimit(0.0f, false);
    }

    // ボスステージのフェード処理
    if(auto boss_stage = Scene::Object::Get<BossStage>())
    {
        auto spherical = boss_stage->GetComponent<SphericalMaskTransition>();
        if(!spherical)
        {
            spherical = boss_stage->AddComponent<SphericalMaskTransition>();
            spherical->SetMaskInverse(false);
            spherical->SetDestroyLimit(0.0f, true);
        }
    }

    // ボススカイドームのフェード処理
    if(auto sky = Scene::Object::Get<BossSkyDome>())
    {
        auto spherical = sky->GetComponent<SphericalMaskTransition>();
        if(!spherical)
        {
            spherical = sky->AddComponent<SphericalMaskTransition>();
            spherical->SetMaskInverse(false);
            spherical->SetDestroyLimit(0.0f, true);
            spherical->StartTransition();
        }
    }
    if(auto player = info_.player_.lock())
    {
        if(auto anim = player->GetComponent<CutScenePlayerAnimationController>())
        {
            // アニメーションを設定
            anim->SetFlag(CutScenePlayerAnimationController::STATE_BIT::LOOK_AROUND);
        }
    }

    if(auto camera = info_.camera_.lock())
    {
        if(auto cmp_cam = camera->GetComponent<ComponentCamera>())
        {
            if(auto player = info_.player_.lock())
            {
                // カメラの注視点を設定
                initial_look_target_ = player->GetTranslate() + float3{0.0f, 25.0f, 0.0f};
            }
        }
        if(auto sound = camera->GetComponent<Sound>())
        {
            // 地震音を再生
            sound->PlayNoSame("earthquake");
        }
    }

    // 名前表示
    CreateNameText("オペレーター");

    // テキスト表示
    draw_text_ = info_.owner_->AddComponent<AutoDrawText>();
    messages_  = draw_text_->LoadTextFile("data/Game/TextData/SceneVictoryCutScene.txt");
    draw_text_->SetAlignment(AutoDrawText::TextAlignment::CENTER)
        ->SetText(messages_[current_message_index_])
        ->SetDrawBox({WINDOW_W * 0.4f, WINDOW_H * 0.9f}, {WINDOW_W * 0.6f, WINDOW_H * 0.95f})
        ->SetFontSize(22)
        ->SetDrawSpeed(0.0f)
        ->StartDraw();
    dialogue_voice_ = info_.owner_->AddComponent<DialogueVoice>();
    dialogue_voice_->SetSequence("SceneVictoryCutScene", current_message_index_);
    dialogue_voice_->Play();
}

//! @brief 更新
void CutSceneStageFade::Update()
{
    Super::Update();
    auto player = info_.player_.lock();
    auto camera = info_.camera_.lock();

    if(!player || !camera)
    {
        return;
    }
    scene_time_ += GetDeltaTime();

    if(camera_state_ != CameraState::COMPLETE)
    {
        // カメラの注視点移動処理
        float t = scene_time_ / ROTATION_DURATION_;
        t       = Clamp01(t);

        float3 target_pos;

        // カメラの状態遷移
        if(camera_state_ == CameraState::TO_START)
        {
            // 開始位置へ移動
            target_pos = lerp(initial_look_target_, aim_start_pos_, t);

            if(scene_time_ >= ROTATION_DURATION_ && (!dialogue_voice_ || !dialogue_voice_->IsPlaying()))
            {
                camera_state_ = CameraState::TO_END;
                scene_time_   = 0.0f;
                MoveNextText();
            }
        }
        else if(camera_state_ == CameraState::TO_END)
        {
            // 終了位置へ移動
            target_pos = lerp(aim_start_pos_, aim_end_pos_, t);

            if(scene_time_ >= ROTATION_DURATION_ && (!dialogue_voice_ || !dialogue_voice_->IsPlaying()))
            {
                camera_state_ = CameraState::TO_INIT;
                scene_time_   = 0.0f;
                MoveNextText();
            }
        }
        else if(camera_state_ == CameraState::TO_INIT)
        {
            // 初期位置へ移動
            target_pos = lerp(aim_end_pos_, initial_look_target_, t);

            if(scene_time_ >= ROTATION_DURATION_ && (!dialogue_voice_ || !dialogue_voice_->IsPlaying()))
            {
                camera_state_ = CameraState::COMPLETE;
                target_pos    = initial_look_target_;
                scene_time_   = 0.0f;
                MoveNextText();
            }
        }
        // カメラの注視点の移動
        camera->SetRotationToPosition(target_pos, float3{0.0f, 1.0f, 0.0f}, true);
    }
    else
    {
        if(auto anim = player->GetComponent<CutScenePlayerAnimationController>())
        {
            // 走り去るアニメーションを設定
            anim->ResetFlag(CutScenePlayerAnimationController::STATE_BIT::LOOK_AROUND);
            anim->SetFlag(CutScenePlayerAnimationController::STATE_BIT::RUN_BACK);
        }
        // 黒画面フェードイン
        auto black_screen = Scene::Object::Get<BlackScreen>();
        if(!black_screen)
        {
            black_screen = Scene::Object::Create<BlackScreen>("BlackScreen")->SetFadeTime(1.0f);
            black_screen->StartFadeIn();
        }
        else if(black_screen->IsFadeComplete())
        {
            // 次のカットシーンへ遷移
            ChangeCutScene<CutSceneRunEscape>();
        }
    }
}

//! @brief 遅延描画
void CutSceneStageFade::LateDraw()
{
    Super::LateDraw();
}

//! @brief GUI表示
void CutSceneStageFade::GUI()
{
    Super::GUI();
    ImGui::Begin(info_.owner_->GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("CutSceneStageFade");
    }
    ImGui::End();
}

//! @brief 終了処理
void CutSceneStageFade::Exit()
{
    Super::Exit();

    if(dialogue_voice_)
    {
        dialogue_voice_->Stop();
        info_.owner_->RemoveComponent(dialogue_voice_);
        dialogue_voice_.reset();
    }
    // 名前テキスト削除
    ClearNameText();

    // テキスト削除
    if(draw_text_)
    {
        info_.owner_->RemoveComponent(draw_text_);
        draw_text_ = nullptr;
    }
    if(auto camera = info_.camera_.lock())
    {
        if(auto sound = camera->GetComponent<Sound>())
        {
            // 音を停止
            sound->Stop("earthquake");
        }
    }
}

//! @brief 次のテキストへ移動
void CutSceneStageFade::MoveNextText()
{
    // サイズを超えていなければ
    if(current_message_index_ < messages_.size() - 1)
    {
        // 次のメッセージへ
        current_message_index_++;
        if(draw_text_)
        {
            draw_text_->SetText(messages_[current_message_index_])->StartDraw();
            dialogue_voice_->PlayMessage(current_message_index_);
        }
    }
}

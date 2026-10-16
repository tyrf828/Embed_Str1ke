//===========================================================================
//! @file CutSceneRunEscape.cpp
//! @brief プレイヤーが出口に逃げているカットシーン
//===========================================================================
#include "CutSceneRunEscape.h"
#include <Game/Component/DialogueVoice.h>
#include "System/Component/ComponentModel.h"
#include <Game/Object/BlackScreen.h>
#include <Game/Component/Sound.h>
#include <Game/Component/CutScene/Animation/CutScenePlayerAnimationController.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <Game/Object/Crystal.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Object/Truck.h>
#include <Game/Component/CutScene/Victory/CutSceneEscape.h>
#include <Game/Component/UI/AutoDrawText.h>

//! @brief 初期化
//! @param info カットシーンの保護情報
void CutSceneRunEscape::Init(const ProtectInfo& info)
{
    CutSceneBase::Init(info);

    // フェードアウト処理
    if(auto black_screen = Scene::Object::Get<BlackScreen>("BlackScreen"))
    {
        black_screen->SetFadeTime(0.5f)->StartFadeOut(true);
    }
    else
    {
        Scene::Object::Create<BlackScreen>("BlackScreen")->SetFadeTime(1.5f)->StartFadeOut(true);
    }

    // プレイヤー初期位置
    const float3 PLAYER_INIT_POS = float3{-875.0f, 0.0f, -300.0f};
    // FPS視点のプレイヤーを作成
    player_fps_body_ = Scene::Object::Create<Object>("Player_FPS_Body");
    player_fps_body_->SetTranslate(PLAYER_INIT_POS);
    player_fps_body_->SetRotationAxisXYZ(float3{0.0f, 180.0f, 0.0f});

    player_fps_body_->AddComponent<ComponentCollisionCapsule>()->SetHeight(15.0f)->SetRadius(3.0f)->UseGravity();

    // トラックを作成
    Scene::Object::Create<Truck>("Truck")->SetTranslate(float3{-875.0f, -5.0f, 64.0f})->SetRotationAxisXYZ(float3{0.0f, 180.0f, 0.0f});

    if(auto camera = info_.camera_.lock())
    {
        // カメラの位置と回転を設定
        camera->SetTranslate(PLAYER_INIT_POS + float3{0.0f, 20.0f, 0.0f});
        camera->SetRotationAxisXYZ(player_fps_body_->GetRotationAxisXYZ());
        if(auto sound = camera->GetComponent<Sound>())
        {
            // 音を再生
            sound->PlayNoSame("run", DX_PLAYTYPE_LOOP);
            sound->PlayNoSame("breath", DX_PLAYTYPE_LOOP);
        }
    }

    CreateNameText("オペレーター");

    // テキスト表示
    draw_text_ = info_.owner_->AddComponent<AutoDrawText>();
    draw_text_->SetAlignment(AutoDrawText::TextAlignment::CENTER)
        ->SetText("急いで")
        ->SetDrawBox({WINDOW_W * 0.4f, WINDOW_H * 0.9f}, {WINDOW_W * 0.6f, WINDOW_H * 0.95f})
        ->SetFontSize(22)
        ->SetDrawSpeed(0.0f)
        ->StartDraw();

    // セリフ音声を再生
    dialogue_voice_ = info_.owner_->AddComponent<DialogueVoice>();
    dialogue_voice_->SetSequence("CutSceneEscape", 0);
    dialogue_voice_->Play();
}

//! @brief 更新
void CutSceneRunEscape::Update()
{
    Super::Update();
    auto player = info_.player_.lock();
    auto camera = info_.camera_.lock();

    if(!player || !camera)
    {
        return;
    }

    // フェードが完了するまで待機
    if(auto screen = Scene::Object::Get<BlackScreen>("BlackScreen"))
    {
        if(!screen->IsFadeComplete())
        {
            return;
        }
    }

    if(player_fps_body_)
    {
        // プレイヤーを前進させる
        player_fps_body_->AddTranslate(float3{0.0f, 0.0f, 1.0f});
        camera->SetTranslate(player_fps_body_->GetTranslate() + float3{0.0f, 20.0f, 0.0f});

        // ヘッドノッド
        scene_time_ += GetDeltaTime();

        float horizontal_scale = 2.0f;   // 左右の揺れ幅
        float vertical_scale   = 1.5f;   // 上下の幅

        // カメラの注視点の基準位置
        float3 center = camera->GetTranslate() + float3{0.0f, 0.0f, 100.0f};

        // カメラの揺れを計算
        float x      = sinf(scene_time_ * 0.5f * HEAD_NOD_SPEED_) * horizontal_scale;
        float y      = -abs(sinf(scene_time_ * HEAD_NOD_SPEED_)) * vertical_scale;
        look_target_ = center + float3{x, y, 0.0f};

        camera->SetRotationToPosition(look_target_, float3{0.0f, 1.0f, 0.0f}, true);
    }

    // 一定距離進んだら次のカットシーンへ
    if(player_fps_body_->GetTranslate().z >= 0.0f)
    {
        ChangeCutScene<CutSceneEscape>();
    }
}

//! @brief GUI表示
void CutSceneRunEscape::GUI()
{
    Super::GUI();
    ImGui::Begin(info_.owner_->GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("CutSceneRunEscape");
    }
    ImGui::End();
}

//! @brief 終了処理
void CutSceneRunEscape::Exit()
{
    Super::Exit();
    // オブジェクトを削除
    Scene::Object::Release(player_fps_body_);
    player_fps_body_ = nullptr;

    // 会話音声を停止して削除
    if(dialogue_voice_)
    {
        dialogue_voice_->Stop();
        info_.owner_->RemoveComponent(dialogue_voice_);
        dialogue_voice_.reset();
    }

    // テキスト削除
    ClearNameText();
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
            sound->Stop("run");
            sound->Stop("breath");
        }
    }
}

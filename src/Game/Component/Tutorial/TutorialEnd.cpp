//===========================================================================
//! @file TutorialEnd.cpp
//! @brief チュートリアル終了
//===========================================================================
#include "TutorialEnd.h"
#include <Game/Object/BlackScreen.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/UI/AutoDrawText.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Object/Player/CameraAimPoint.h>
#include <Game/Component/ComponentCameraController.h>
#include <Game/Component/Tutorial/TutorialMove.h>
#include <System/Component/ComponentCollisionBox.h>
#include <Game/Scene/SceneGame.h>

//! @brief 初期化
void TutorialEnd::Init(const ProtectInfo& info)
{
    TutorialBase::Init(info);

    // テキスト設定
    text_ = info_.owner_->AddComponent<AutoDrawText>();
    text_->SetDrawBox(message_box_top_left_, message_box_bottom_right_);
    text_->SetFontSize(24);

    // テキスト読み込み
    messages_              = text_->LoadTextFile("data/Game/TutorialTextData/TutorialEnd.txt");
    current_message_index_ = 0;
    text_->SetText(messages_[current_message_index_]);
    dialogue_voice_->SetSequence("TutorialEnd", current_message_index_);

    text_box_scale_ = {1.0f, 0.0f};
}

//! @brief 更新
void TutorialEnd::Update()
{
    TutorialBase::Update();
    // テキストボックスアニメーション
    MainTextBoxAnimation();

    // テキストボックスを表示
    if(text_box_scale_.y >= 1.0f)
    {
        if(current_message_index_ == 0)
        {
            StopPlayerControl(true);
            text_->StartDraw(false);
            dialogue_voice_->Play();
        }
    }
    else if(is_close_text_box_)
    {
        //フェードイン処理
        auto black_screen = Scene::Object::Get<BlackScreen>();
        if(!black_screen)
        {
            black_screen = Scene::Object::Create<BlackScreen>();
            black_screen->StartFadeIn(false);
        }
        if(black_screen->IsFadeComplete())
        {
            // シーン切り替え
            Scene::Change(Scene::GetScene<SceneGame>());
        }
        return;
    }
    // メッセージ入力処理
    if(IsPressMessageKey())
    {
        ProcessMessageInput(text_, current_message_index_, TEXT_SPEED_);
    }
    // 全メッセージ表示完了処理
    if(current_message_index_ >= messages_.size())
    {
        StopPlayerControl(false);
        DeleteAllText();
        is_close_text_box_ = true;
    }
}

//! @brief 遅延描画
void TutorialEnd::LateDraw()
{
    Super::LateDraw();
    if(!is_close_text_box_)
    {
        // オペレーター画像表示
        DrawNaviOperator();
    }
    // メッセージボックス描画
    DrawRotaGraph3F(WINDOW_W * 0.5f,
                    WINDOW_H * 0.85f,
                    info_.box_size_buffer_x_ * 0.5f,
                    info_.box_size_buffer_y_ * 0.5f,
                    text_box_scale_.x * SCALE_WINDOW,
                    text_box_scale_.y * SCALE_WINDOW,
                    0.0f,
                    info_.message_box_handle_,
                    true);
}

//! @brief GUI表示処理
void TutorialEnd::GUI()
{
    Super::GUI();
    ImGui::Begin(info_.owner_->GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("TutorialEnd");
    }
    ImGui::End();
}

//! @brief 終了
void TutorialEnd::Exit()
{
    Super::Exit();
    StopPlayerControl(false);
    if(auto player = info_.player_.lock())
    {
        player->SetHPBottleAmount(player->GetHPBottleMax());
    }
}

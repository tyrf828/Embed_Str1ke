//===========================================================================
//! @file TutorialStart.cpp
//! @brief チュートリアルスタート
//===========================================================================
#include "TutorialStart.h"
#include <Game/Object/Player/Player.h>
#include <Game/Component/UI/AutoDrawText.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Object/Player/CameraAimPoint.h>
#include <Game/Component/ComponentCameraController.h>
#include <Game/Component/Tutorial/TutorialMove.h>
#include <System/Component/ComponentCollisionBox.h>
#include <Game/Object/Enemy/CloseEnemy/CloseEnemy.h>

//! @brief 初期化
void TutorialStart::Init(const ProtectInfo& info)
{
    TutorialBase::Init(info);
    StopPlayerControl(true);
    info_.tutorail_wall_->SetTranslate(float3{-485.0f, 135.0f, -313.0f});
    info_.tutorail_wall_->GetComponent<ComponentCollisionBox>()->SetSize(float3{50.0f, 80.0f, 100.0f});

    auto_draw_text_ = info_.owner_->AddComponent<AutoDrawText>();
    // テキストファイルを読み込む
    messages_ = auto_draw_text_->LoadTextFile("data/Game/TutorialTextData/TutorialStart.txt");
    auto_draw_text_->SetDrawBox(message_box_top_left_, message_box_bottom_right_);
    auto_draw_text_->SetFontSize(24);

    // 最初のメッセージを表示
    current_message_index_ = 0;
    auto_draw_text_->SetText(messages_[current_message_index_]);
    dialogue_voice_->SetSequence("TutorialStart", current_message_index_);
    auto_draw_text_->StartDraw();
    dialogue_voice_->Play();

    text_box_scale_ = {1.0f, 1.0f};
}

//! @brief 更新
void TutorialStart::Update()
{
    TutorialBase::Update();
    MainTextBoxAnimation();

    // メッセージ送り
    if(IsPressMessageKey())
    {
        ProcessMessageInput(auto_draw_text_, current_message_index_, TEXT_SPEED_);
        if(auto_draw_text_)
        {
            // 特定のメッセージでカメラを揺らす
            if(!auto_draw_text_->IsDrawComplete() && auto_draw_text_->GetDrawSpeed() > 0)
            {
                if(current_message_index_ == 1 || current_message_index_ == 5)
                {
                    info_.camera_.lock()->ShakeCamera(0.5f, (float)current_message_index_);
                }
            }
        }
        if(current_message_index_ >= messages_.size())
        {
            is_close_text_box_ = true;
        }
    }

    // 終了処理
    if(is_close_text_box_)
    {
        text_box_scale_.y -= GetDeltaTime();
        text_box_scale_.y  = std::max(0.0f, (float)text_box_scale_.y);
        if(text_box_scale_.y <= 0.0f)
        {
            StopPlayerControl(false);
            clear_elasped_time_ += GetDeltaTime();
        }
        if(clear_elasped_time_ >= 1.0f)
        {
            ChangeTutorialStep<TutorialMove>();
            return;
        }
    }
}

//! @brief 遅延描画
void TutorialStart::LateDraw()
{
    Super::LateDraw();
    if(!is_close_text_box_)
    {
        // ナビゲーター画像描画
        DrawNaviOperator();
    }
    int size_x_buf = info_.box_size_buffer_x_;
    int size_y_buf = info_.box_size_buffer_y_;
    DrawRotaGraph3F(WINDOW_W * 0.5f,
                    WINDOW_H * 0.85f,
                    (float)size_x_buf * 0.5f,
                    (float)size_y_buf * 0.5f,
                    text_box_scale_.x * SCALE_WINDOW,
                    text_box_scale_.y * SCALE_WINDOW,
                    0.0f,
                    info_.message_box_handle_,
                    true);
}

//! @brief GUI表示処理
void TutorialStart::GUI()
{
    Super::GUI();
    ImGui::Begin(info_.owner_->GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("TutorialStart");
    }
    ImGui::End();
}

//! @brief 終了
void TutorialStart::Exit()
{
    Super::Exit();

    auto_draw_text_ = nullptr;
}

//===========================================================================
//! @file TutorialMove.cpp
//! @brief チュートリアル移動
//===========================================================================
#include "TutorialMove.h"
#include <Game/Object/BlackScreen.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/UI/AutoDrawText.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Object/Player/CameraAimPoint.h>
#include <Game/Component/ComponentCameraController.h>
#include <System/Utils/IniFileLib.h>
#include <Game/Component/Player/Move.h>
#include <Game/Component/Tutorial/TutorialAttack.h>

//! @brief 初期化
void TutorialMove::Init(const ProtectInfo& info)
{
    TutorialBase::Init(info);

    message_box_top_left_     = {WINDOW_W * 0.4f, WINDOW_H * 0.77f};
    message_box_bottom_right_ = {WINDOW_W * 0.7f, WINDOW_H * 0.83f};
    text_box_scale_           = {0.8f, 0.0f};

    // 画像を読み込む
    if(GetConnecetdPadNum() == -1)
    {
        move_ghandle_ = LoadGraph("data/Game/GHandle/Keyboard & Mouse/Default/keyboard_wasd.png");
        run_ghandle_  = LoadGraph("data/Game/GHandle/Keyboard & Mouse/Default/keyboard_space.png");
    }
    else
    {
        move_ghandle_ = LoadGraph("data/Game/GHandle/Xbox Series/Default/xbox_stick_l.png");
        run_ghandle_  = LoadGraph("data/Game/GHandle/Xbox Series/Default/xbox_button_color_a.png");
    }

    // テキストファイルを読み込む
    message_text_ = info_.owner_->AddComponent<AutoDrawText>()
                        ->SetDrawBox(message_box_top_left_, message_box_bottom_right_)
                        ->SetFontSize(24)
                        ->SetTextData("data/Game/TutorialTextData/TutorialMove.txt");
    dialogue_voice_->SetSequence("TutorialMove", 0, 1);

    IniFileLib ini("Game/TutorialTextData/TutorialQuest.txt");
    {
        std::string str  = ini.GetString("Move", "Move");
        quest_move_text_ = info_.owner_->AddComponent<AutoDrawText>()
                               ->SetDrawBox({WINDOW_W * 0.85f, WINDOW_H * 0.07f}, {WINDOW_W * 0.95f, WINDOW_H * 0.1f})
                               ->SetFontSize(22)
                               ->SetText(str);
    }
    {
        std::string str = ini.GetString("Move", "Run");
        quest_run_text_ = info_.owner_->AddComponent<AutoDrawText>()
                              ->SetDrawBox({WINDOW_W * 0.84f, WINDOW_H * 0.15f}, {WINDOW_W * 0.98f, WINDOW_H * 0.20f})
                              ->SetFontSize(22)
                              ->SetText(str);
    }
}

//! @brief 更新
void TutorialMove::Update()
{
    TutorialBase::Update();

    // テキストボックスのアニメーション
    if(!is_close_text_box_)
    {
        text_box_scale_.y += GetDeltaTime();
        text_box_scale_.y  = std::min(SUB_TEXT_BOX_MAX_.y, text_box_scale_.y);
    }
    else
    {
        // クエストボックスが閉じたら次のステップへ
        if(quest_box_scale_.y <= 0.0f)
        {
            ChangeTutorialStep<TutorialAttack>();
            return;
        }
    }

    // クエストボックスのアニメーション
    QuestBoxAnimation(is_close_text_box_);
    if(quest_box_scale_.y >= QUEST_BOX_MAX_.y)
    {
        if(message_text_)
        {
            message_text_->StartDraw(false);
            dialogue_voice_->Play();
        }
    }

    // クエストテキストの表示開始
    float draw_speed = 0.05f;
    if(quest_move_text_)
    {
        draw_speed = quest_move_text_->GetDrawSpeed();
    }

    if(quest_elasped_time_ >= draw_speed)
    {
        if(quest_move_text_)
        {
            quest_move_text_->StartDraw(false);
        }
        if(quest_run_text_)
        {
            quest_run_text_->StartDraw(false);
        }
    }

    // プレイヤーの行動判定
    if(auto player = info_.player_.lock())
    {
        auto move = player->GetComponent<Move>();
        if(move && quest_move_text_ && quest_run_text_)
        {
            // 移動判定
            if(move->IsMoving())
            {
                if(quest_move_text_->IsDrawComplete())
                {
                    quest_move_text_->SetColor(CLEAR_COLOR_);
                }
                // 走り判定
                if(player->IsRun())
                {
                    if(quest_run_text_->IsDrawComplete())
                    {
                        quest_run_text_->SetColor(CLEAR_COLOR_);
                    }
                }
            }
        }
    }
    // クエストをクリア
    if(quest_move_text_ && quest_run_text_)
    {
        if(quest_move_text_->GetTextColor() == CLEAR_COLOR_ && quest_run_text_->GetTextColor() == CLEAR_COLOR_)
        {
            clear_elasped_time_ += GetDeltaTime();
        }
    }
    // クリア後の遷移処理
    if(clear_elasped_time_ >= 1.0f)
    {
        is_close_text_box_ = true;
        if(quest_move_text_)
        {
            info_.owner_->RemoveComponent(quest_move_text_);
            quest_move_text_.reset();
        }
        if(quest_run_text_)
        {
            info_.owner_->RemoveComponent(quest_run_text_);
            quest_run_text_.reset();
        }
    }
}

//! @brief 遅延描画
void TutorialMove::LateDraw()
{
    Super::LateDraw();

    {
        DrawQuestBox(quest_box_scale_);
        if(draw_quest_ghandle_ && !is_close_text_box_)
        {
            DrawRotaGraphF((float)WINDOW_W * 0.8f, WINDOW_H * 0.09f, 1.0f * SCALE_WINDOW, 0.0f, move_ghandle_, true);
            DrawRotaGraphF((float)WINDOW_W * 0.8f, WINDOW_H * 0.17f, 1.0f * SCALE_WINDOW, 0.0f, run_ghandle_, true);
        }
    }

    {
        DrawQuestMessageBox(text_box_scale_);
    }
}

//! @brief GUI表示処理
void TutorialMove::GUI()
{
    Super::GUI();
    ImGui::Begin(info_.owner_->GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("TutorialMove");
    }
    ImGui::End();
}

//! @brief 終了
void TutorialMove::Exit()
{
    Super::Exit();

    DeleteGraph(move_ghandle_);
    DeleteGraph(run_ghandle_);
    DeleteAllText();
    message_text_    = nullptr;
    quest_move_text_ = nullptr;
    quest_run_text_  = nullptr;
}

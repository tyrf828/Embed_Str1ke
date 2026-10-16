//===========================================================================
//! @file TutorialRegen.cpp
//! @brief チュートリアル回復
//===========================================================================
#include "TutorialRegen.h"
#include <Game/Object/BlackScreen.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/UI/AutoDrawText.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Object/Player/CameraAimPoint.h>
#include <Game/Component/ComponentCameraController.h>
#include <System/Utils/IniFileLib.h>
#include <Game/Component/Player/Move.h>
#include <Game/Component/Tutorial/TutorialLock.h>
#include <System/Component/ComponentCollisionBox.h>

//! @brief 初期化
void TutorialRegen::Init(const ProtectInfo& info)
{
    TutorialBase::Init(info);
    message_box_top_left_     = {WINDOW_W * 0.33f, WINDOW_H * 0.77f};
    message_box_bottom_right_ = {WINDOW_W * 0.73f, WINDOW_H * 0.84f};

    // 画像を読み込む
    if(GetConnecetdPadNum() == -1)
    {
        regen_ghandle_ = LoadGraph("data/Game/GHandle/Keyboard & Mouse/Default/keyboard_r.png");
    }
    else
    {
        regen_ghandle_ = LoadGraph("data/Game/GHandle/Xbox Series/Default/xbox_lb.png");
    }

    // テキストファイルを読み込む
    message_text_ = info_.owner_->AddComponent<AutoDrawText>()
                        ->SetDrawBox(message_box_top_left_, message_box_bottom_right_)
                        ->SetFontSize(22)
                        ->SetTextData("data/Game/TutorialTextData/TutorialRegen.txt");
    dialogue_voice_->SetSequence("TutorialRegen", 0, 1);

    IniFileLib ini("Game/TutorialTextData/TutorialQuest.txt");
    std::string str   = ini.GetString("Regen", "Regen");
    quest_regen_text_ = info_.owner_->AddComponent<AutoDrawText>()
                            ->SetDrawBox({WINDOW_W * 0.84f, WINDOW_H * 0.11f}, {WINDOW_W * 0.98f, WINDOW_H * 0.20f})
                            ->SetFontSize(22)
                            ->SetText(str);

    // 回復アイテムを最大にする
    if(auto player = info_.player_.lock())
    {
        player->SetHPBottleAmount(player->GetHPBottleMax());
    }

    text_box_scale_.y = 0.0f;
}

//! @brief 更新
void TutorialRegen::Update()
{
    TutorialBase::Update();
    MainTextBoxAnimation(SUB_TEXT_BOX_MAX_.y);

    // テキストボックスの表示制御
    if(!is_close_text_box_)
    {
        if(text_box_scale_.y >= SUB_TEXT_BOX_MAX_.y)
        {
            message_text_->StartDraw(false);
            dialogue_voice_->Play();
        }
    }
    else
    {
        // 終了処理
        dialogue_voice_->Stop();
        info_.owner_->RemoveComponent(message_text_);
        message_text_ = nullptr;
        if(quest_box_scale_.y <= 0.0f)
        {
            ChangeTutorialStep<TutorialLock>();
            return;
        }
    }

    // クエストボックスのアニメーション
    QuestBoxAnimation(is_close_text_box_);
    if(quest_regen_text_)
    {
        if(quest_elasped_time_ >= quest_regen_text_->GetDrawSpeed())
        {
            quest_regen_text_->StartDraw(false);
        }
    }
    else
    {
        return;
    }

    // 回復判定
    if(auto player = info_.player_.lock())
    {
        if(player->IsRegenerateHP())
        {
            if(quest_regen_text_->IsDrawComplete())
            {
                quest_regen_text_->SetColor(CLEAR_COLOR_);
            }
        }
    }
    QuestClear(quest_regen_text_);
}

//! @brief 遅延描画
void TutorialRegen::LateDraw()
{
    Super::LateDraw();
    DrawQuestBox(quest_box_scale_);
    if(draw_quest_ghandle_ && !is_close_text_box_)
    {
        DrawRotaGraphF((float)WINDOW_W * 0.8f, (float)WINDOW_H * 0.13f, 0.7f * SCALE_WINDOW, 0.0f, regen_ghandle_, true);
    }

    DrawQuestMessageBox(text_box_scale_);
}

//! @brief GUI表示処理
void TutorialRegen::GUI()
{
    Super::GUI();
    ImGui::Begin(info_.owner_->GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("TutorialRegen");
    }
    ImGui::End();
}

//! @brief 終了
void TutorialRegen::Exit()
{
    Super::Exit();
    DeleteGraph(regen_ghandle_);
    message_text_ = nullptr;
    info_.tutorail_wall_->GetComponent<ComponentCollisionBox>()->SetCollisionStatus(ComponentCollision::CollisionBit::DisableHit, true);
}

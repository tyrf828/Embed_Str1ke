//===========================================================================
//! @file TutorialAttack.cpp
//! @brief チュートリアル攻撃
//===========================================================================
#include "TutorialAttack.h"
#include <Game/Object/BlackScreen.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/UI/AutoDrawText.h>
#include <System/Utils/IniFileLib.h>
#include <Game/Component/Player/Attack3.h>
#include <Game/Component/Tutorial/TutorialAttachExplain.h>
#include <System/Component/ComponentCollisionBox.h>

//! @brief 初期化
void TutorialAttack::Init(const ProtectInfo& info)
{
    TutorialBase::Init(info);
    message_box_top_left_     = {WINDOW_W * 0.29f, WINDOW_H * 0.75f};
    message_box_bottom_right_ = {WINDOW_W * 0.73f, WINDOW_H * 0.84f};

    // 攻撃ボタン画像の読み込み
    if(GetConnecetdPadNum() == -1)
    {
        attack_ghandle_ = LoadGraph("data/Game/GHandle/Keyboard & Mouse/Default/mouse_left.png");
    }
    else
    {
        attack_ghandle_ = LoadGraph("data/Game/GHandle/Xbox Series/Default/xbox_button_color_x.png");
    }

    // テキストファイルを読み込む
    message_text_ = info_.owner_->AddComponent<AutoDrawText>()
                        ->SetDrawBox(message_box_top_left_, message_box_bottom_right_)
                        ->SetFontSize(22)
                        ->SetTextData("data/Game/TutorialTextData/TutorialAttack.txt");
    dialogue_voice_->SetSequence("TutorialAttack", 0, 2);
    message_text_->StartDraw();
    dialogue_voice_->Play();

    // クエストテキストの読み込みと設定
    IniFileLib ini("Game/TutorialTextData/TutorialQuest.txt");
    std::string str    = ini.GetString("Attack", "Attack");
    quest_attack_text_ = info_.owner_->AddComponent<AutoDrawText>()
                             ->SetDrawBox({WINDOW_W * 0.84f, WINDOW_H * 0.11f}, {WINDOW_W * 0.98f, WINDOW_H * 0.20f})
                             ->SetFontSize(22)
                             ->SetDrawSpeed(TEXT_SPEED_)
                             ->SetText(str);
}

//! @brief 更新
void TutorialAttack::Update()
{
    TutorialBase::Update();
    // クエストボックスのアニメーション更新
    QuestBoxAnimation(is_close_text_box_);

    // クエストボックスが閉じたら次のチュートリアルステップへ移行
    if(quest_box_scale_.y <= 0.0f)
    {
        ChangeTutorialStep<TutorialAttachExplain>();
        return;
    }

    if(quest_attack_text_)
    {
        if(quest_elasped_time_ >= quest_attack_text_->GetDrawSpeed())
        {
            quest_attack_text_->StartDraw(false);
        }
    }

    // プレイヤーのコンボ攻撃が完了したらクエストテキストをクリアの色にする
    if(auto player = info_.player_.lock())
    {
        if(auto attack3 = player->GetComponent<Attack3>())
        {
            if(quest_attack_text_->IsDrawComplete())
            {
                quest_attack_text_->SetColor(CLEAR_COLOR_);
            }
        }
    }

    // クエストクリア処理
    QuestClear(quest_attack_text_);
}

//! @brief 遅延描画
void TutorialAttack::LateDraw()
{
    Super::LateDraw();
    DrawQuestBox(quest_box_scale_);
    if(draw_quest_ghandle_ && !is_close_text_box_)
    {
        DrawRotaGraphF((float)WINDOW_W * 0.8f, (float)WINDOW_H * 0.13f, 1.0f * SCALE_WINDOW, 0.0f, attack_ghandle_, true);
    }

    DrawQuestMessageBox(text_box_scale_);
}

//! @brief GUI表示処理
void TutorialAttack::GUI()
{
    Super::GUI();
    ImGui::Begin(info_.owner_->GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("TutorialAttack");
    }
    ImGui::End();
}

//! @brief 終了
void TutorialAttack::Exit()
{
    Super::Exit();
    // 画像の削除
    DeleteGraph(attack_ghandle_);
    // テキストの削除
    DeleteAllText();
    message_text_      = nullptr;
    quest_attack_text_ = nullptr;
    info_.tutorail_wall_->GetComponent<ComponentCollisionBox>()->SetCollisionStatus(ComponentCollision::CollisionBit::DisableHit, true);
}

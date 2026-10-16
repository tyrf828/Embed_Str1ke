//===========================================================================
//! @file TutorialAttachAttack.cpp
//! @brief チュートリアルアタッチ攻撃
//===========================================================================
#include "TutorialAttachAttack.h"
#include <Game/Object/Player/Player.h>
#include <Game/Component/UI/AutoDrawText.h>
#include <Game/Component/ComponentCameraController.h>
#include <System/Utils/IniFileLib.h>
#include <Game/Object/Weapon/AttachWeapon/Barrel.h>
#include <Game/Object/Weapon/AttachWeapon/Box.h>
#include <Game/Object/Weapon/AttachWeapon/TrafficCone.h>
#include <Game/Object/Weapon/AttachWeapon/BarrelWet.h>
#include <Game/Component/Weapon/AttachWeapon.h>
#include <Game/Component/Tutorial/TutorialRegen.h>

//! @brief 初期化
void TutorialAttachAttack::Init(const ProtectInfo& info)
{
    TutorialBase::Init(info);

    // アタッチボタンのグラフィック読み込み
    if(GetConnecetdPadNum() == -1)
    {
        attach_ghandle_ = LoadGraph("data/Game/GHandle/Keyboard & Mouse/Default/mouse_right.png");
        scan_ghandle_   = LoadGraph("data/Game/GHandle/Keyboard & Mouse/Default/keyboard_q.png");
    }
    else
    {
        attach_ghandle_ = LoadGraph("data/Game/GHandle/Xbox Series/Default/xbox_button_color_b.png");
        scan_ghandle_   = LoadGraph("data/Game/GHandle/Xbox Series/Default/xbox_button_color_a.png");
    }

    // クエストテキストの読み込みと設定
    IniFileLib ini("Game/TutorialTextData/TutorialQuest.txt");
    {
        std::string str    = ini.GetString("Attach", "Attach");
        quest_attach_text_ = info_.owner_->AddComponent<AutoDrawText>()
                                 ->SetDrawBox({WINDOW_W * 0.84f, WINDOW_H * 0.08f}, {WINDOW_W * 0.98f, WINDOW_H * 0.20f})
                                 ->SetFontSize(22)
                                 ->SetDrawSpeed(TEXT_SPEED_)
                                 ->SetText(str);
    }

    {
        std::string str  = ini.GetString("Attach", "Scan");
        quest_scan_text_ = info_.owner_->AddComponent<AutoDrawText>()
                               ->SetDrawBox({WINDOW_W * 0.8f, WINDOW_H * 0.15f}, {WINDOW_W * 0.98f, WINDOW_H * 0.2f})
                               ->SetFontSize(20)
                               ->SetDrawSpeed(TEXT_SPEED_)
                               ->SetText(str);
    }

    // アタッチ武器の取得
    attach_weapon_ = {
        Scene::Object::Get<Barrel>(),
        Scene::Object::Get<Box>(),
        Scene::Object::Get<TrafficCone>(),
        Scene::Object::Get<BarrelWet>(),
    };
}

//! @brief 更新
void TutorialAttachAttack::Update()
{
    TutorialBase::Update();

    // クエストボックスのアニメーション更新
    QuestBoxAnimation(is_close_text_box_);

    // クエストテキストの表示開始
    float draw_speed = 0.05f;
    if(quest_attach_text_)
    {
        draw_speed = quest_attach_text_->GetDrawSpeed();
    }

    if(quest_elasped_time_ >= draw_speed)
    {
        if(quest_attach_text_)
        {
            quest_attach_text_->StartDraw(false);
        }
        if(quest_scan_text_)
        {
            quest_scan_text_->StartDraw(false);
        }
    }

    // クエストボックスが閉じたら次のチュートリアルステップへ移行
    if(quest_box_scale_.y <= 0)
    {
        ChangeTutorialStep<TutorialRegen>();
        return;
    }

    // アタッチ武器がすべてアタッチされているか確認
    int attached_count = 0;
    for(auto& weapon : attach_weapon_)
    {
        if(auto attach = weapon->GetComponent<AttachWeapon>())
        {
            attached_count++;
            // すべてアタッチされたらクエストクリア
            if(attach_weapon_.size() == attached_count)
            {
                if(quest_attach_text_->IsDrawComplete())
                {
                    quest_attach_text_->SetColor(CLEAR_COLOR_);
                    quest_scan_text_->SetColor(CLEAR_COLOR_);
                }
            }
        }
    }

    // クエストクリア処理
    QuestClear(quest_attach_text_);
    QuestClear(quest_scan_text_);
}

//! @brief 遅延描画
void TutorialAttachAttack::LateDraw()
{
    Super::LateDraw();

    DrawQuestBox(quest_box_scale_);
    if(draw_quest_ghandle_ && !is_close_text_box_)
    {
        DrawRotaGraphF((float)WINDOW_W * 0.8f, (float)WINDOW_H * 0.1f, 1.0f * SCALE_WINDOW, 0.0f, attach_ghandle_, true);
        int width  = 0;
        int height = 0;
        GetGraphSize(scan_ghandle_, &width, &height);
        int font_handle = quest_scan_text_->GetFontHandle();

        float string_x = WINDOW_W * 0.78f - width * 0.8f * SCALE_WINDOW * 0.5f - GetDrawStringWidthToHandle("（", 2, font_handle);
        DrawStringFToHandle(string_x, WINDOW_H * 0.15f, "（", quest_scan_text_->GetTextColor(), font_handle);

        DrawRotaGraphF((float)WINDOW_W * 0.78f, (float)WINDOW_H * 0.17f, 0.8f * SCALE_WINDOW, 0.0f, scan_ghandle_, true);

        std::string scan_text = quest_scan_text_->GetText();

        string_x = WINDOW_W * 0.8f + GetDrawStringWidthToHandle(scan_text.c_str(), (int)scan_text.size(), font_handle);
        DrawStringFToHandle(string_x, WINDOW_H * 0.15f, "）", quest_scan_text_->GetTextColor(), font_handle);
    }
}

//! @brief GUI表示処理
void TutorialAttachAttack::GUI()
{
    Super::GUI();
    ImGui::Begin(info_.owner_->GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("TutorialAttachAttack");
    }
    ImGui::End();
}

//! @brief 終了
void TutorialAttachAttack::Exit()
{
    Super::Exit();
    DeleteGraph(attach_ghandle_);
    DeleteGraph(scan_ghandle_);
}

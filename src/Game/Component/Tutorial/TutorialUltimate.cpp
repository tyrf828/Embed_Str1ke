//===========================================================================
//! @file TutorialUltimate.cpp
//! @brief チュートリアル必殺技
//===========================================================================
#include "TutorialUltimate.h"
#include <Game/Object/Player/Player.h>
#include <Game/Component/UI/AutoDrawText.h>
#include <System/Utils/IniFileLib.h>
#include <Game/Component/Weapon/AttachWeapon.h>
#include <Game/Component/Player/UltimateGauge.h>
#include <Game/Component/Player/UltimateAttack/UltimateAttack.h>
#include <Game/Object/Enemy/CloseEnemy/CloseEnemy.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyAction.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyIdle.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Component/ComponentCameraController.h>
#include <Game/Component/Tutorial/TutorialLock.h>
#include <Game/Object/BlackScreen.h>
#include <System/Component/ComponentModel.h>
#include <Game/Component/Tutorial/TutorialEnd.h>

//! @brief 初期化
void TutorialUltimate::Init(const ProtectInfo& info)
{
    TutorialBase::Init(info);

    // 画像を読み込む
    if(GetConnecetdPadNum() == -1)
    {
        ult_ghandle_  = LoadGraph("data/Game/GHandle/Keyboard & Mouse/Default/keyboard_v.png");
        lock_ghandle_ = LoadGraph("data/Game/GHandle/Keyboard & Mouse/Default/keyboard_f.png");
    }
    else
    {
        ult_ghandle_  = LoadGraph("data/Game/GHandle/Xbox Series/Default/xbox_button_color_y.png");
        lock_ghandle_ = LoadGraph("data/Game/GHandle/Xbox Series/Default/xbox_rb.png");
    }
    // クエストテキスト設定
    IniFileLib ini("Game/TutorialTextData/TutorialQuest.txt");
    {
        std::string str = ini.GetString("Ult", "Ult");
        quest_ult_text_ = info_.owner_->AddComponent<AutoDrawText>()
                              ->SetDrawBox({WINDOW_W * 0.84f, WINDOW_H * 0.07f}, {WINDOW_W * 0.98f, WINDOW_H * 0.1f})
                              ->SetFontSize(22)
                              ->SetText(str);
    }
    {
        std::string str  = ini.GetString("Lock", "Lock");
        quest_lock_text_ = info_.owner_->AddComponent<AutoDrawText>()
                               ->SetDrawBox({WINDOW_W * 0.84f, WINDOW_H * 0.15f}, {WINDOW_W * 0.98f, WINDOW_H * 0.20f})
                               ->SetFontSize(22)
                               ->SetText(str);
    }
    if(auto player = info_.player_.lock())
    {
        player_start_pos_ = player->GetTranslate();
        if(auto model = player->GetComponent<ComponentModel>())
        {
            player_start_rot_ = model->GetRotationAxisXYZ();
        }
    }
    // 敵を取得
    auto enemies = Scene::Object::GetArray<CloseEnemy>();
    for(auto& enemy : enemies)
    {
        enemy_.push_back(enemy);
    }
}

//! @brief 更新
void TutorialUltimate::Update()
{
    Super::Update();
    // クエストボックスアニメーション
    QuestBoxAnimation(is_close_text_box_);

    // クエストテキスト表示
    if(quest_ult_text_)
    {
        if(quest_elasped_time_ >= quest_ult_text_->GetDrawSpeed())
        {
            quest_ult_text_->StartDraw(false);
            if(quest_lock_text_)
            {
                quest_lock_text_->StartDraw(false);
            }
        }
    }
    else
    {
        ChangeTutorialStep<TutorialEnd>();
        return;
    }

    if(auto player = info_.player_.lock())
    {
        auto ult = player->GetComponent<UltimateAttack>();
        if(ult)
        {
            // 必殺技発動中
            if(quest_ult_text_)
            {
                quest_ult_text_->SetColor(CLEAR_COLOR_);
            }
        }
        else
        {
            if(is_success_)
            {
                // 成功したら、次のステップへ移行
                ChangeTutorialStep<TutorialEnd>();
                return;
            }
            // ゲージ増加
            if(auto gauge = player->GetComponent<UltimateGauge>())
            {
                gauge->AddEnergy((int)(gauge->GetUltimateCost() * GetDeltaTime()));
            }
        }

        // 必殺技成功判定
        if(quest_ult_text_)
        {
            if(quest_ult_text_->GetTextColor() == CLEAR_COLOR_ && !ult)
            {
                int success_count = 0;
                for(auto& enemy : enemy_)
                {
                    if(auto action = enemy->GetComponent<CloseEnemyAction>())
                    {
                        auto now = action->GetCurrentAction();
                        if(std::dynamic_pointer_cast<CloseEnemyIdle>(now))
                        {
                            break;
                        }
                    }
                    success_count++;
                }
                // 全ての敵を倒したか
                if(success_count >= enemy_.size())
                {
                    is_success_ = true;
                }
                else
                {
                    // 失敗時はリセット
                    player->SetTranslate(player_start_pos_);
                    if(auto model = player->GetComponent<ComponentModel>())
                    {
                        model->SetRotationAxisXYZ(player_start_rot_);
                    }

                    enemy_.clear();
                    ChangeTutorialStep<TutorialLock>();
                    Scene::Object::Create<BlackScreen>()->StartFadeOut(true);
                    return;
                }
            }
        }
    }

    // ロックオン判定
    if(auto camera = info_.camera_.lock())
    {
        if(camera->IsCameraLock() && quest_lock_text_)
        {
            quest_lock_text_->SetColor(CLEAR_COLOR_);
        }
    }
}

//! @brief 遅延描画
void TutorialUltimate::LateDraw()
{
    Super::LateDraw();

    DrawQuestBox(quest_box_scale_);
    if(draw_quest_ghandle_ && !is_close_text_box_)
    {
        DrawRotaGraphF((float)WINDOW_W * 0.8f, (float)WINDOW_H * 0.09f, 1.0f * SCALE_WINDOW, 0.0f, ult_ghandle_, true);
        DrawRotaGraphF((float)WINDOW_W * 0.8f, (float)WINDOW_H * 0.17f, 1.0f * SCALE_WINDOW, 0.0f, lock_ghandle_, true);
    }
}

//! @brief GUI表示処理
void TutorialUltimate::GUI()
{
    Super::GUI();
    ImGui::Begin(info_.owner_->GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("TutorialUltimate");
    }
    ImGui::End();
}

//! @brief 終了
void TutorialUltimate::Exit()
{
    Super::Exit();
    DeleteGraph(ult_ghandle_);
    DeleteGraph(lock_ghandle_);
    DeleteAllText();
    quest_ult_text_  = nullptr;
    quest_lock_text_ = nullptr;
    for(auto& enemy : enemy_)
    {
        Scene::Object::Release(enemy);
    }
    if(auto player = info_.player_.lock())
    {
        if(auto gauge = player->GetComponent<UltimateGauge>())
        {
            gauge->SetEnergy(0);
        }
    }
}

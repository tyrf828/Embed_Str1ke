//===========================================================================
//! @file TutorialLock.cpp
//! @brief チュートリアルカメラロック
//===========================================================================
#include "TutorialLock.h"
#include <Game/Object/BlackScreen.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/UI/AutoDrawText.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Object/Player/CameraAimPoint.h>
#include <Game/Component/ComponentCameraController.h>
#include <System/Utils/IniFileLib.h>
#include <Game/Component/Player/Move.h>
#include <Game/Object/Manager/SpawnManager.h>
#include <Game/Object/Enemy/CloseEnemy/CloseEnemy.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyAction.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyIdle.h>
#include <System/Component/ComponentCollisionBox.h>
#include <Game/Component/Tutorial/TutorialUltimate.h>

//! @brief 初期化
void TutorialLock::Init(const ProtectInfo& info)
{
    TutorialBase::Init(info);

    auto enemies = Scene::Object::GetArray<CloseEnemy>();
    enemy_.fill(nullptr);
    int enemy_index = 0;
    for(auto& enemy : enemies)
    {
        if(enemy_index >= enemy_.size())
        {
            break;
        }
        if(enemy->GetStatus(Object::StatusBit::Alive) && !enemy->GetStatus(Object::StatusBit::Exited) && enemy->GetComponent<CloseEnemyAction>())
        {
            enemy_[enemy_index] = enemy;
            enemy_index++;
        }
    }
    for(; enemy_index < enemy_.size(); enemy_index++)
    {
        enemy_[enemy_index] = Scene::Object::Create<CloseEnemy>();
    }

    // メッセージテキスの読み込み
    message_text_ = info_.owner_->AddComponent<AutoDrawText>();
    messages_     = message_text_->LoadTextFile("data/Game/TutorialTextData/TutorialLock.txt");
    message_text_->SetDrawBox(message_box_top_left_, message_box_bottom_right_);
    message_text_->SetFontSize(24);
    message_text_->SetText(messages_[current_message_index_]);
    dialogue_voice_->SetSequence("TutorialLock", current_message_index_);

    text_box_scale_ = {1.0f, 0.0f};

    // 表示カメラを作成
    show_camera_ =
        Scene::Object::Create<Object>("TutorialAttachShowCamera")->SetTranslate(float3{40.5f, 136.5f, -331.0})->SetRotationAxisXYZ(float3{-6.0f, 63.0f, 0.0f});
    show_camera_->AddComponent<ComponentCamera>();

    // チュートリアル用壁の設定
    info_.tutorail_wall_->SetTranslate(float3{45.0f, 93.0f, -280.0f});
    if(auto col = info_.tutorail_wall_->GetComponent<ComponentCollisionBox>())
    {
        col->SetSize(float3{15.0f, 20.0f, 200.0f});
        col->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER);
        col->SetOverlapCollisionGroup((u32)ComponentCollision::CollisionGroup::CAMERA);
        col->SetCollisionStatus(ComponentCollision::CollisionBit::DisableHit, false);
    }
}

//! @brief 更新
void TutorialLock::Update()
{
    TutorialBase::Update();
    // メインテキストボックスアニメーション
    MainTextBoxAnimation();

    // テキストボックスが閉じ終わったら次のチュートリアルへ
    if(is_close_text_box_)
    {
        if(text_box_scale_.y <= 0.0f)
        {
            ChangeTutorialStep<TutorialUltimate>();
            return;
        }
    }

    if(text_box_scale_.y >= 1.0f)
    {
        // メッセージ開始処理
        if(current_message_index_ == 0)
        {
            StopPlayerControl(true);
            message_text_->StartDraw(false);
            dialogue_voice_->Play();
        }
        // メッセージキー入力処理
        if(IsPressMessageKey())
        {
            ProcessMessageInput(message_text_, current_message_index_, TEXT_SPEED_);
            if(current_message_index_ == 1)
            {
                // 敵の表示処理
                show_camera_->GetComponent<ComponentCamera>()->SetCurrentCamera();
                for(int i = 0; i < enemy_.size(); i++)
                {
                    enemy_[i]->ResetObjectInfo();
                    enemy_[i]->GetComponent<CloseEnemyAction>()->SetAction(std::make_shared<CloseEnemyIdle>());
                    const float POS_Z    = -275.0f;
                    const float OFFSET_Z = -20.0f;
                    enemy_[i]->SetTranslate(float3{110.0f, 92.0f, POS_Z - i * OFFSET_Z});
                    const float MID_POS_Z = (POS_Z * (float)enemy_.size() - OFFSET_Z) * 0.5f;
                    enemy_[i]->SetRotationToPosition(float3{115.0f, -100.0f, MID_POS_Z});
                }
                auto black_screen = Scene::Object::Get<BlackScreen>();
                if(!black_screen)
                {
                    // フェードアウト開始
                    black_screen = Scene::Object::Create<BlackScreen>();
                    black_screen->StartFadeOut();
                }
            }
        }
    }
    // メッセージ全部表示終わったら
    if(current_message_index_ >= messages_.size())
    {
        if(message_text_)
        {
            dialogue_voice_->Stop();
            info_.owner_->RemoveComponent(message_text_);
        }
        if(auto black_screen = Scene::Object::Get<BlackScreen>())
        {
            // フェードアウト
            if(black_screen->IsFadeComplete())
            {
                black_screen->StartFadeOut(true);
            }
        }
        if(auto camera = info_.camera_.lock())
        {
            // カメラを元に戻す
            if(auto cmp_camera = camera->GetComponent<ComponentCamera>())
            {
                cmp_camera->SetCurrentCamera();
            }
        }
        // プレイヤー操作再開
        StopPlayerControl(false);
        if(auto camera_aim = Scene::Object::Get<CameraAimPoint>())
        {
            if(auto cam_control = camera_aim->GetComponent<ComponentCameraController>())
            {
                // カメラの操作を再開
                cam_control->SetUseInput(true);
            }
        }
        is_close_text_box_ = true;
    }
}

//! @brief 遅延描画
void TutorialLock::LateDraw()
{
    Super::LateDraw();

    if(text_box_scale_.y >= 1.0f)
    {
        // ナビオペレーター画像を描画
        DrawNaviOperator();
    }

    int size_x_buf = info_.box_size_buffer_x_;
    int size_y_buf = info_.box_size_buffer_y_;
    // メッセージボックスを描画
    DrawRotaGraph3F(WINDOW_W * 0.5f,
                    WINDOW_H * 0.85f,
                    size_x_buf * 0.5f,
                    size_y_buf * 0.5f,
                    text_box_scale_.x * SCALE_WINDOW,
                    text_box_scale_.y * SCALE_WINDOW,
                    0.0f,
                    info_.message_box_handle_,
                    true);
}

//! @brief GUI表示処理
void TutorialLock::GUI()
{
    Super::GUI();
    ImGui::Begin(info_.owner_->GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("TutorialLock");
    }
    ImGui::End();
}

//! @brief 終了
void TutorialLock::Exit()
{
    Super::Exit();
    // 表示カメラ解放
    Scene::Object::Release(show_camera_);
    show_camera_ = nullptr;
}

//===========================================================================
//! @file TutorialAttachExplain.cpp
//! @brief チュートリアルアタッチ攻撃説明
//===========================================================================
#include "TutorialAttachExplain.h"
#include <Game/Object/BlackScreen.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/UI/AutoDrawText.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Object/Player/CameraAimPoint.h>
#include <Game/Component/ComponentCameraController.h>
#include <System/Utils/IniFileLib.h>
#include <Game/Component/Player/Move.h>
#include <Game/Object/Weapon/AttachWeapon/Barrel.h>
#include <Game/Object/Weapon/AttachWeapon/Box.h>
#include <Game/Object/Weapon/AttachWeapon/TrafficCone.h>
#include <Game/Object/Weapon/AttachWeapon/BarrelWet.h>
#include <Game/Object/Manager/SpawnManager.h>
#include <System/Component/ComponentCamera.h>
#include <System/Component/ComponentCollisionBox.h>
#include <Game/Component/Tutorial/TutorialAttachAttack.h>
#include <Game/Object/Weapon/AttachWeapon/AttachWeaponBase.h>

//! @brief 初期化
void TutorialAttachExplain::Init(const ProtectInfo& info)
{
    TutorialBase::Init(info);
    // スポーンマネージャーの作成
    spawn_manager_ = Scene::Object::Create<SpawnManager>("ItemSpawnManager")->AddSpawners("data/Game/SpawnerData/SceneGameTutorialSpawner.txt");
    for(auto& weapon : Scene::Object::GetArray<AttachWeaponBase>())
    {
        spawn_manager_->AddSpawnPool(weapon);
    }
    spawn_manager_->SpawnProcess();
    message_text_ = info_.owner_->AddComponent<AutoDrawText>();
    // テキストファイルを読み込む
    messages_ = message_text_->LoadTextFile("data/Game/TutorialTextData/TutorialAttach.txt");
    message_text_->SetDrawBox(message_box_top_left_, message_box_bottom_right_);
    message_text_->SetFontSize(24);
    message_text_->SetText(messages_[current_message_index_]);
    dialogue_voice_->SetSequence("TutorialAttach", current_message_index_);

    text_box_scale_ = {1.0f, 0.0f};

    // 表示カメラの作成
    show_camera_ = Scene::Object::Create<Object>("TutorialAttachShowCamera")
                       ->SetTranslate(float3{-306.0f, 116.0f, -287.0f})
                       ->SetRotationAxisXYZ(float3{-7.0f, 20.0f, 0.0f});
    show_camera_->AddComponent<ComponentCamera>()->SetPerspective(68.0f);

    // チュートリアル用の壁を設置
    info_.tutorail_wall_->SetTranslate(float3{-150.0f, 104.0f, -280.0f});
    if(auto box = info_.tutorail_wall_->GetComponent<ComponentCollisionBox>())
    {
        box->SetSize(float3{50.0f, 80.0f, 200.0f});
        box->SetCollisionStatus(ComponentCollision::CollisionBit::DisableHit, false);
    }
}

//! @brief 更新
void TutorialAttachExplain::Update()
{
    TutorialBase::Update();
    // テキストボックスアニメーション
    MainTextBoxAnimation();
    if(text_box_scale_.y >= 0.3f)
    {
        // プレイヤー操作停止
        StopPlayerControl(true);

        if(current_message_index_ == 0)
        {
            message_text_->StartDraw(false);
            dialogue_voice_->Play();
        }
        // メッセージキー入力処理
        if(IsPressMessageKey())
        {
            ProcessMessageInput(message_text_, current_message_index_, TEXT_SPEED_);
        }
        // メッセージに応じた処理
        if(current_message_index_ == 1)
        {
            if(!transition_screen_)
            {
                transition_screen_ = Scene::Object::Create<BlackScreen>();
                transition_screen_->StartFadeOut(false);
            }
            // カメラ切り替え
            show_camera_->GetComponent<ComponentCamera>()->SetCurrentCamera();
        }
    }
    if(current_message_index_ >= messages_.size())
    {
        transition_screen_->StartFadeOut(true);
        transition_screen_.reset();
        ChangeTutorialStep<TutorialAttachAttack>();
        return;
    }
}

//! @brief 遅延描画
void TutorialAttachExplain::LateDraw()
{
    Super::LateDraw();
    if(text_box_scale_.y >= 1.0f)
    {
        // ナビオペレーター画像描画
        DrawNaviOperator();
    }

    int size_x_buf = info_.box_size_buffer_x_;
    int size_y_buf = info_.box_size_buffer_y_;
    // メッセージボックス描画
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
void TutorialAttachExplain::GUI()
{
    Super::GUI();
    ImGui::Begin(info_.owner_->GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("TutorialAttachExplain");
    }
    ImGui::End();
}

//! @brief 終了
void TutorialAttachExplain::Exit()
{
    Super::Exit();
    // プレイヤ操作再開
    StopPlayerControl(false);

    // カメラを元に戻す
    if(auto camera = info_.camera_.lock())
    {
        if(auto cmp = camera->GetComponent<ComponentCamera>())
        {
            cmp->SetCurrentCamera();
        }
    }

    if(transition_screen_)
    {
        Scene::Object::Release(transition_screen_);
        transition_screen_.reset();
    }

    // オブジェクト解放
    Scene::Object::Release(show_camera_);
    show_camera_ = nullptr;
    Scene::Object::Release(spawn_manager_);
    spawn_manager_ = nullptr;
    DeleteAllText();
    message_text_ = nullptr;
}

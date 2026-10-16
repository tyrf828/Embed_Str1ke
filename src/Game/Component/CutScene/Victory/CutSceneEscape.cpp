//===========================================================================
//! @file CutSceneEscape.cpp
//! @brief プレイヤーが脱出したカットシーン
//===========================================================================
#include "CutSceneEscape.h"
#include <Game/Component/DialogueVoice.h>
#include "System/Component/ComponentModel.h"
#include <Game/Component/CutScene/EnterBoss/CutSceneBossLook.h>
#include <Game/Object/BlackScreen.h>
#include <Game/Component/Sound.h>
#include <Game/Component/CutScene/Animation/CutScenePlayerAnimationController.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <Game/Object/Crystal.h>
#include <Game/Object/Truck.h>
#include <Game/Scene/SceneTitle.h>
#include <Game/Component/UI/AutoDrawText.h>

//! @brief 初期化
//! @param info カットシーンの保護情報
void CutSceneEscape::Init(const ProtectInfo& info)
{
    CutSceneBase::Init(info);

    // 名前表示
    CreateNameText("オペレーター");

    // テキスト表示
    draw_text_ = info_.owner_->AddComponent<AutoDrawText>();
    draw_text_->SetAlignment(AutoDrawText::TextAlignment::CENTER)
        ->SetText("お疲れ様")
        ->SetDrawBox({WINDOW_W * 0.4f, WINDOW_H * 0.9f}, {WINDOW_W * 0.6f, WINDOW_H * 0.95f})
        ->SetFontSize(22)
        ->SetDrawSpeed(0.0f)
        ->StartDraw();

    // 表示したセリフに対応する音声を再生
    dialogue_voice_ = info_.owner_->AddComponent<DialogueVoice>();
    dialogue_voice_->SetSequence("CutSceneEscape", 1);
    dialogue_voice_->Play();
}

//! @brief 更新
void CutSceneEscape::Update()
{
    Super::Update();
    auto player = info_.player_.lock();
    auto camera = info_.camera_.lock();

    if(!player || !camera)
    {
        return;
    }
    auto truck = Scene::Object::Get<Truck>("Truck");
    switch(state_)
    {
    case State::LOOK_UP:
        // 上を見る
        camera_look_pos_ = camera->GetTranslate() + float3{0.0f, 0.15f, 1.0f};
        // カメラを回転させる
        camera->SetRotationToVectorWithLimit(camera_look_pos_ - camera->GetTranslate(), 0.3f, float3{0.0f, 1.0f, 0.0f}, true);
        // 回転が完了したら次の状態へ
        if(abs(camera->GetRotationAxisXYZ().x - camera_prev_rot_.x) <= (float1)FLT_EPSILON)
        {
            state_ = State::CLIMB_UP;
        }
        camera_prev_rot_ = camera->GetRotationAxisXYZ();
        break;
    case State::CLIMB_UP:
        {
            // トラックに登る
            camera_look_pos_ = {-875.0f, 19.0f, 25.0f};
            // カメラを回転させる
            camera->SetRotationToPositionWithLimit(camera_look_pos_, 1.0f, camera->GetVectorAxisY(), true);

            // カメラを移動させる
            const float3 velocity = -camera->GetVectorAxisZ() * 0.3f;
            camera->AddTranslate(velocity);

            // 目標地点に到達したら次の状態へ
            if(length(camera->GetTranslate() - camera_look_pos_) <= length(velocity))
            {
                state_           = State::TRUCK_RUN;
                camera_prev_rot_ = float3{0.0f, 0.0f, 0.0f};
            }
            if(auto sound = camera->GetComponent<Sound>())
            {
                // 音を再生
                sound->PlayOnce("climb up");
            }
        }
        break;
    case State::TRUCK_RUN:
        {
            if(scene_time_ == 0.0f)
            {
                // 次のセリフと音声へ切り替える
                draw_text_->SetText("これで任務クリア")->StartDraw();
                dialogue_voice_->PlayMessage(2);

                // 初回のみカメラとトラックの差分を計算
                camera_in_truck_pos_ = camera->GetTranslate() - truck->GetTranslate();
                camera_in_truck_rot_ = camera->GetRotationAxisXYZ() - truck->GetRotationAxisXYZ();
            }

            // カメラをトラックの方向に回転させる
            camera_look_pos_ = camera->GetTranslate() + -truck->GetVectorAxisZ() + -truck->GetVectorAxisX();
            camera->SetRotationToPositionWithLimit(camera_look_pos_, 0.5f, camera->GetVectorAxisY(), true);

            // トラックを回転・移動
            scene_time_ += GetDeltaTime();
            truck->AddRotationAxisXYZ(float3{0.0f, scene_time_ * 0.15f, 0.0f});
            truck->AddTranslate(-truck->GetVectorAxisZ());
            if(auto truck_se = truck->GetComponent<Sound>())
            {
                // 音を再生
                truck_se->PlayOnce("truck_start");
            }

            // トラックに追従させる
            float3 x            = normalize(truck->GetVectorAxisX()) * camera_in_truck_pos_.x;
            float3 y            = normalize(truck->GetVectorAxisY()) * camera_in_truck_pos_.y;
            float3 z            = -normalize(truck->GetVectorAxisZ()) * camera_in_truck_pos_.z;
            float3 world_offset = x + y + z;
            camera->SetTranslate(truck->GetTranslate() + world_offset);

            // トラックが一定角度回転したら完了状態へ
            if(truck->GetRotationAxisXYZ().y >= -90.0f)
            {
                state_      = State::COMPLETED;
                scene_time_ = 0.0f;
            }
        }
        break;
    case State::COMPLETED:
        {
            // 完了処理
            float3 x            = normalize(truck->GetVectorAxisX()) * camera_in_truck_pos_.x;
            float3 y            = normalize(truck->GetVectorAxisY()) * camera_in_truck_pos_.y;
            float3 z            = -normalize(truck->GetVectorAxisZ()) * camera_in_truck_pos_.z;
            float3 world_offset = x + y + z;
            camera->SetTranslate(truck->GetTranslate() + world_offset);
            truck->AddTranslate(-truck->GetVectorAxisZ());
            auto black_screen = Scene::Object::Get<BlackScreen>("BlackScreen");
            if(!black_screen)
            {
                // 最後の音声が終わるまで字幕を残す
                if(dialogue_voice_ && dialogue_voice_->IsPlaying())
                {
                    break;
                }
                // フェードアウト
                black_screen = Scene::Object::Create<BlackScreen>("BlackScreen")->SetFadeTime(2.0f);
                black_screen->StartFadeIn(false);

                // テキスト削除
                ClearNameText();
                draw_text_->SetText("")->StartDraw();
            }
            else
            {
                // フェード完了
                if(black_screen->IsFadeComplete())
                {
                    // 一定時間経過でタイトル画面へ遷移
                    scene_time_ += GetDeltaTime();
                    if(scene_time_ >= 2.0f)
                    {
                        Scene::Change(Scene::GetScene<SceneTitle>());
                    }
                }
            }
        }
        break;
    default:
        break;
    }
}

//! @brief GUI表示
void CutSceneEscape::GUI()
{
    Super::GUI();
    ImGui::Begin(info_.owner_->GetName().data());
    {
        ImGui::Separator();
        ImGui::Text("CutSceneEscape");
    }
    ImGui::End();
}

//! @brief 終了処理
void CutSceneEscape::Exit()
{
    Super::Exit();

    // 会話音声を停止して削除
    if(dialogue_voice_)
    {
        dialogue_voice_->Stop();
        info_.owner_->RemoveComponent(dialogue_voice_);
        dialogue_voice_.reset();
    }

    // テキスト削除
    if(draw_text_)
    {
        info_.owner_->RemoveComponent(draw_text_);
        draw_text_ = nullptr;
    }
}

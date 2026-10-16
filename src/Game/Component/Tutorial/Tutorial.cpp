//===========================================================================
//! @file Tutorial.cpp
//! @brief チュートリアルコンポーネント
//===========================================================================
#include "Tutorial.h"
#include "TutorialStart.h"
#include "TutorialMove.h"
#include "TutorialAttack.h"
#include "TutorialAttachExplain.h"
#include "TutorialAttachAttack.h"
#include "TutorialRegen.h"
#include "TutorialLock.h"
#include "TutorialUltimate.h"
#include "TutorialEnd.h"
#include <Game/Component/UI/AutoDrawText.h>
#include <System/Component/ComponentCollisionBox.h>
#include <Game/Object/Player/CameraAimPoint.h>
#include <Game/Component/ComponentCameraController.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/Player/AttackBase.h>

//! @brief 初期化
void Tutorial::Init()
{
    Super::Init();
    // チュートリアル情報初期化
    info_.owner_              = GetOwnerPtr();
    info_.message_box_handle_ = LoadGraph("data/Game/GHandle/box_blue.png");
    info_.navi_handle_        = LoadGraph("data/Game/GHandle/navi.png");
    info_.tutorail_wall_      = Scene::Object::Create<Object>("TutorialWall");
    info_.tutorail_wall_->AddComponent<ComponentCollisionBox>()
        ->SetCollisionGroup(ComponentCollision::CollisionGroup::GROUND)
        ->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER | (u32)ComponentCollision::CollisionGroup::ETC)
        ->SetMass(-1.0f);
    GetGraphSize(info_.message_box_handle_, &info_.box_size_buffer_x_, &info_.box_size_buffer_y_);
    // チュートリアルステップ開始
    SetState(std::make_shared<TutorialStart>());

    SetPriority(ProcTiming::LateDraw, ProcPriority::LOW);
}

//! @brief 更新
void Tutorial::Update()
{
    Super::Update();
    if(auto state = now_state_)
    {
        state->Update();
    }
}

//! @brief 遅延描画
void Tutorial::LateDraw()
{
    Super::LateDraw();
    if(now_state_)
    {
        now_state_->LateDraw();
    }
}

//! @brief GUI表示処理
void Tutorial::GUI()
{
    Super::GUI();
    if(now_state_)
    {
        now_state_->GUI();
    }
}

//! @brief 終了
void Tutorial::Exit()
{
    Super::Exit();
    if(now_state_)
    {
        now_state_->Exit();
    }
    DeleteGraph(info_.message_box_handle_);
    DeleteGraph(info_.navi_handle_);
}

//! @brief クエストボックスを描画
//! @param box_scale ボックスのスケール
void TutorialBase::DrawQuestBox(float2 box_scale)
{
    DrawRotaGraph3F((float)WINDOW_W,
                    0.0f,
                    (float)info_.box_size_buffer_x_,
                    0.0f,
                    box_scale.x * SCALE_WINDOW,
                    box_scale.y * SCALE_WINDOW,
                    0.0f,
                    info_.message_box_handle_,
                    true);
}

//! @brief クエストメッセージボックスを描画
//! @param box_scale ボックスのスケール
void TutorialBase::DrawQuestMessageBox(float2 box_scale)
{
    float size_buffer_x = info_.box_size_buffer_x_ * 0.5f;
    float size_buffer_y = info_.box_size_buffer_y_ * 0.5f;
    DrawRotaGraph3F(WINDOW_W * 0.5f,
                    WINDOW_H * 0.78f,
                    size_buffer_x,
                    size_buffer_y,
                    box_scale.x * SCALE_WINDOW,
                    box_scale.y * SCALE_WINDOW,
                    0.0f,
                    info_.message_box_handle_,
                    true);
}

//! @brief クエストボックスのアニメーション
//! @param is_close 閉じるかどうか
void TutorialBase::QuestBoxAnimation(bool is_close)
{
    if(!is_close)
    {
        quest_box_scale_.y += GetDeltaTime();
        quest_box_scale_.y  = std::min(QUEST_BOX_MAX_.y, quest_box_scale_.y);
    }
    else
    {
        quest_box_scale_.y -= GetDeltaTime();
        quest_box_scale_.y  = std::max(0.0f, (float)quest_box_scale_.y);
    }
    if(quest_box_scale_.y >= QUEST_BOX_MAX_.y)
    {
        quest_elasped_time_ += GetDeltaTime();
        draw_quest_ghandle_  = true;
    }
}

//! @brief 全てのテキストを削除
void TutorialBase::DeleteAllText()
{
    auto text = info_.owner_->GetComponents<AutoDrawText>();
    for(auto& t : text)
    {
        info_.owner_->RemoveComponent(t);
    }
}

//! @brief ナビオペレーターを描画
void TutorialBase::DrawNaviOperator()
{
    DrawRotaGraphF(WINDOW_W * 0.75f, WINDOW_H * 0.65f, 0.4f * SCALE_WINDOW, 0.0f, info_.navi_handle_, true);
}

//! @brief クエストクリア処理
//! @param text_ptr テキストポインタ
//! @param close_time テキストボックスを閉じるまでの時間
void TutorialBase::QuestClear(AutoDrawTextPtr text_ptr, float close_time)
{
    if(text_ptr->GetTextColor() == CLEAR_COLOR_)
    {
        clear_elasped_time_ += GetDeltaTime();
        if(clear_elasped_time_ >= close_time)
        {
            is_close_text_box_ = true;
            info_.owner_->RemoveComponent(text_ptr);
        }
    }
}

//! @brief プレイヤー操作を停止
//! @param is_stop 停止するかどうか
void TutorialBase::StopPlayerControl(bool is_stop)
{
    if(auto player = info_.player_.lock())
    {
        player->SetActionEnable(!is_stop);
        if(auto attack_base = player->GetComponent<AttackBase>())
        {
            attack_base->DisableAttackButton(is_stop);
        }
    }
    if(auto camera_aim = Scene::Object::Get<CameraAimPoint>())
    {
        if(auto cam_control = camera_aim->GetComponent<ComponentCameraController>())
        {
            cam_control->SetUseInput(!is_stop);
        }
    }
}

//! @brief メインテキストボックスのアニメーション
//! @param max_scale_y 最大スケールY
void TutorialBase::MainTextBoxAnimation(float max_scale_y)
{
    if(!is_close_text_box_)
    {
        text_box_scale_.y += GetDeltaTime();
        text_box_scale_.y  = std::min(max_scale_y, (float)text_box_scale_.y);
    }
    else
    {
        text_box_scale_.y -= GetDeltaTime();
        text_box_scale_.y  = std::max(0.0f, (float)text_box_scale_.y);
    }
}

//! @brief メッセージ入力処理
//! @param message_text メッセージテキストポインタ
//! @param current_message_index 現在のメッセージインデックス
//! @param text_speed テキスト描画速度
void TutorialBase::ProcessMessageInput(AutoDrawTextPtr& message_text, int& current_message_index, float text_speed)
{
    if(!message_text)
    {
        return;
    }

    // テキスト描画が完了していない場合は、スピードを0にして即座に表示
    if(!message_text->IsDrawComplete())
    {
        message_text->SetDrawSpeed(0.0f);
        return;
    }
    else
    {
        // 次のメッセージ用に速度を元に戻す
        message_text->SetDrawSpeed(text_speed);
    }

    current_message_index++;

    // 次のメッセージがある場合
    if(current_message_index < messages_.size())
    {
        message_text->SetText(messages_[current_message_index]);
        dialogue_voice_->PlayMessage(current_message_index);
        message_text->StartDraw();
        return;
    }
    else
    {
        dialogue_voice_->Stop();
        // すべてのメッセージを表示し終わった
        info_.owner_->RemoveComponent(message_text);
        message_text = nullptr;
        return;
    }
}

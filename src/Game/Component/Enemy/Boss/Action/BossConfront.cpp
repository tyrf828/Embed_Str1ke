//===========================================================================
//! @file BossConfront.cpp
//! @brief ボスの対峙コンポーネント
//===========================================================================
#include "BossConfront.h"
#include <Game/Component/Enemy/Boss/Action/BossConfrontStay.h>
#include <Game/Component/Enemy/Boss/Action/BossConfrontLeftRight.h>
#include <Game/Component/Enemy/Boss/Action/BossConfrontForward.h>
#include <Game/Component/Enemy/Boss/Action/BossConfrontBackward.h>
#include <Game/Component/Enemy/Boss/BossAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/Player/Move.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
void BossConfront::Init()
{
    Super::Init();

    // プレイヤーが正面にいない場合、後退する
    if(!IsPlayerInFront())
    {
        SetConfrontAction(ConfrontType::BACKWARD);
        return;
    }
    // 一定確率で後退する
    if(GetPlayerDistance() <= CLOSE_DISTANCE_)
    {
        int rand = GetRand(100) + 1;
        if(rand < BACKWARD_RATE_)
        {
            SetConfrontAction(ConfrontType::BACKWARD);
        }
        else
        {
            SetConfrontAction(ConfrontType::STAY);
        }
        return;
    }

    // プレイヤーとの角度が大きい場合、左右移動する
    if(GetPlayerDegree() > 45.0f)
    {
        SetConfrontAction(ConfrontType::LEFT_RIGHT);
        key_timer_ = 1.0f;
        return;
    }

    // 通常は停止する
    SetConfrontAction(ConfrontType::STAY);
}

//! @brief 更新
void BossConfront::Update()
{
    Super::Update();

    auto player      = GetPlayer();
    auto player_move = player->GetComponent<Move>();
    if(!player || !player_move)
    {
        return;
    }

    // プレイヤーの移動を確認
    float3 player_prev_dir = player->GetTranslate() - player_prev_sec_pos_;
    bool player_is_moving  = length(player_prev_dir) > (float1)FLT_EPSILON;

    // プレイヤーが移動した場合
    if(player_is_moving)
    {
        // プレイヤーの移動に応じて左右移動に替える
        if(player_move->GetKey(Move::WALK_KEY_STATUS::KEY_RIGHT) || player_move->GetKey(Move::WALK_KEY_STATUS::KEY_LEFT))
        {
            key_timer_ += GetDeltaTime();
            // 一定時間キー入力が続いた場合、左右移動に切り替える
            if(key_timer_ >= 1.0f)
            {
                // 左右移動に反応
                if(current_confront_type_ != ConfrontType::LEFT_RIGHT)
                {
                    SetConfrontAction(ConfrontType::LEFT_RIGHT);
                }
                key_timer_ = std::min(key_timer_, 1.0f);
            }
        }
    }
    else if(GetConfrontAction(ConfrontType::LEFT_RIGHT))
    {
        // 左右移動中でプレイヤーが止まった場合、前進に切り替える
        key_timer_ -= GetDeltaTime() * 2;
        key_timer_  = std::max(key_timer_, 0.0f);
        if(key_timer_ <= 0.0f)
        {
            SetConfrontAction(ConfrontType::FORWARD);
        }
    }

    // 前進中でプレイヤーに近づきすぎた場合、ランダムなアクションに切り替える
    if(GetPlayerDistance() <= CLOSE_DISTANCE_ && GetConfrontAction(ConfrontType::FORWARD))
    {
        SetRandomConfrontAction();
    }

    // 音の制御
    if(!GetConfrontAction(ConfrontType::STAY))
    {
        if(auto se = GetOwner()->GetComponent<Sound>())
        {
            se->PlayNoSame("walk");
        }
    }
    else
    {
        if(auto se = GetOwner()->GetComponent<Sound>())
        {
            se->Stop("walk");
        }
    }

    // 対峙アクションの更新
    if(confront_action_)
    {
        confront_action_->ConfrontUpdate();
    }
    // プレイヤーの位置を保存
    player_prev_sec_pos_ = player->GetTranslate();
}

//! @brief 遅延描画
void BossConfront::LateDraw()
{
    Super::LateDraw();
}

//! @brief 更新後処理
void BossConfront::PostUpdate()
{
    Super::PostUpdate();
}

//! @brief 終了
void BossConfront::Exit()
{
    Super::Exit();

    // アニメーションフラグのリセット
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::WALK_RIGHT);
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::WALK_LEFT);
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::WALK_FRONT);
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::WALK_BACK);
    }
    // 移動速度のリセット
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        move->SetVelocity(float3{0.0f, 0.0f, 0.0f});
    }
    // 対峙アクションの終了
    if(confront_action_)
    {
        confront_action_->ConfrontExit();
    }
    // 音の停止
    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        se->Stop("walk");
    }
}

//! @brief GUI表示処理
void BossConfront::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("BossConfront"))
        {
            ImGui::Text(u8"現在のアクション: %d", static_cast<int>(current_confront_type_));

            if(ImGui::Button(u8"停止"))
            {
                SetConfrontAction(ConfrontType::STAY);
            }
            if(ImGui::Button(u8"左右移動"))
            {
                SetConfrontAction(ConfrontType::LEFT_RIGHT);
            }
            if(ImGui::Button(u8"前進"))
            {
                SetConfrontAction(ConfrontType::FORWARD);
            }
            if(ImGui::Button(u8"後退"))
            {
                SetConfrontAction(ConfrontType::BACKWARD);
            }

            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//! @brief 対峙アクションを設定
//! @param type 設定するアクションの種類
void BossConfront::SetConfrontAction(ConfrontType type)
{
    // 現在の対峙アクションを更新
    current_confront_type_ = type;

    std::shared_ptr<BossConfrontFactory> new_action = nullptr;

    switch(type)
    {
    case ConfrontType::STAY:
        new_action = std::make_shared<BossConfrontStay>();
        break;
    case ConfrontType::LEFT_RIGHT:
        new_action = std::make_shared<BossConfrontLeftRight>();
        key_timer_ = 1.0f;
        break;
    case ConfrontType::FORWARD:
        new_action = std::make_shared<BossConfrontForward>();
        break;
    case ConfrontType::BACKWARD:
        new_action = std::make_shared<BossConfrontBackward>();
        break;
    }
    new_action->SetOwner(GetOwner());
    new_action->SetParent(std::static_pointer_cast<BossConfront>(shared_from_this()));
    if(confront_action_)
    {
        confront_action_->ConfrontExit();
    }
    confront_action_ = new_action;
    if(confront_action_)
    {
        confront_action_->ConfrontInit();
    }
}

//! @brief ランダムな対峙アクションを設定
void BossConfront::SetRandomConfrontAction()
{
    int rand = GetRand((int)ConfrontType::MAX - 1);

    SetConfrontAction((ConfrontType)rand);
}

//! @brief プレイヤーが正面にいるか判定
//! @retval true 正面にいる
//! @retval false 正面にいない
bool BossConfront::IsPlayerInFront()
{
    if(auto player = GetPlayer())
    {
        float3 front     = -GetOwner()->GetVectorAxisZ();
        front.y          = 0.0f;
        front            = normalize(front);
        float3 to_player = player->GetTranslate() - GetOwner()->GetTranslate();
        to_player.y      = 0.0f;
        to_player        = normalize(to_player);
        // 内積を計算
        float d = dot(normalize(to_player), front);
        return d > FLT_EPSILON;
    }
    return false;
}

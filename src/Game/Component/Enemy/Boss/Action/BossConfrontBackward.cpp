//===========================================================================
//! @file BossConfrontBackward.cpp
//! @brief ボスの後退コンポーネント
//===========================================================================
#include "BossConfrontBackward.h"
#include <Game/Component/Enemy/Boss/BossAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>

//! @brief 初期化
void BossConfrontBackward::ConfrontInit()
{
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションを設定
        anim_control->SetFlag(BossAnimationController::STATE_BIT::WALK_BACK);
    }
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動速度を変更と保存
        prev_move_speed_ = move->GetSpeed();
        move->SetSpeed(MOVE_SPEED_);
    }
}

//! @brief 更新
void BossConfrontBackward::ConfrontUpdate()
{
    auto player = GetPlayer();
    auto move   = GetOwner()->GetComponent<EnemyMove>();

    if(!player || !move)
    {
        return;
    }

    time_ += GetDeltaTime();
    // 一定時間経過したら停止状態へ
    if(time_ > BACK_TIME_)
    {
        if(auto parent = GetParent())
        {
            parent->SetConfrontAction(ConfrontType::STAY);
            return;
        }
    }
    // プレイヤーから離れる方向へ移動
    float3 back_dir = normalize(GetOwner()->GetTranslate() - player->GetTranslate());
    move->SetVelocity(back_dir);

    // プレイヤーの方向を向く
    GetOwner()->SetRotationToPositionWithLimit(player->GetTranslate(), 4.0f);
}

//! @brief 終了
void BossConfrontBackward::ConfrontExit()
{
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションをリセット
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::WALK_BACK);
    }
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動速度を元に戻す
        move->SetSpeed(prev_move_speed_);
    }
}

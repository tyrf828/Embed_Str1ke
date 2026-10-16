//===========================================================================
//! @file BossConfrontLeftRight.cpp
//! @brief ボスの左右移動コンポーネント
//===========================================================================
#include "BossConfrontLeftRight.h"
#include <Game/Component/Enemy/Boss/BossAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/Player/Move.h>

//! @brief 初期化
void BossConfrontLeftRight::ConfrontInit()
{
    // プレイヤの位置に右か左か判別
    if(IsPlayerInLeftRight(true))
    {
        // 右に移動
        move_to_right_ = true;
    }
    else if(IsPlayerInLeftRight(false))
    {
        // 左に移動
        move_to_right_ = false;
    }
}

//! @brief 更新
void BossConfrontLeftRight::ConfrontUpdate()
{
    auto anim_control = GetOwner()->GetComponent<BossAnimationController>();
    auto player       = GetPlayer();
    auto move         = GetOwner()->GetComponent<EnemyMove>();
    auto player_move  = player->GetComponent<Move>();

    if(!anim_control || !player || !move || !player_move)
    {
        return;
    }

    // プレイヤーが右に移動している場合
    if(move_to_right_)
    {
        // 右に移動
        float3 right_dir = GetRightDirection();
        move->SetVelocity(right_dir);

        anim_control->SetFlag(BossAnimationController::STATE_BIT::WALK_RIGHT);
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::WALK_LEFT);
    }
    // プレイヤーが左に移動している場合
    else
    {
        // 左に移動
        float3 left_dir = -GetRightDirection();
        move->SetVelocity(left_dir);

        anim_control->SetFlag(BossAnimationController::STATE_BIT::WALK_LEFT);
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::WALK_RIGHT);
    }

    // プレイヤーの方向を向く
    GetOwner()->SetRotationToPositionWithLimit(player->GetTranslate(), 3.0f);
}

//! @brief 終了
void BossConfrontLeftRight::ConfrontExit()
{
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションフラグをリセット
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::WALK_RIGHT);
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::WALK_LEFT);
    }
}

//! @brief プレイヤーが左右どちらにいるか判定
//! @param check_right true:右側をチェック, false:左側をチェック
//! @return 指定した方向にいる場合true
bool BossConfrontLeftRight::IsPlayerInLeftRight(bool check_right)
{
    if(auto player = GetPlayer())
    {
        float3 right     = GetRightDirection();
        float3 to_player = player->GetTranslate() - GetOwner()->GetTranslate();
        to_player.y      = 0.0f;
        to_player        = normalize(to_player);
        // 内積を計算
        float d = dot(normalize(to_player), right);

        // 少し余裕を持たせて判定
        if(check_right)
        {
            return d < -0.001f;
        }
        else
        {
            return d > -0.001f;
        }
    }
    return false;
}

//! @brief 右方向ベクトルを取得
//! @return 右方向ベクトル
float3 BossConfrontLeftRight::GetRightDirection() const
{
    // 自分の右方向を取得
    float3 right_dir = -GetOwner()->GetVectorAxisX();
    // 水平方向のみ
    right_dir.y = 0.0f;
    return normalize(right_dir);
}

//===========================================================================
//! @file BossConfrontForward.cpp
//! @brief ボスの前進コンポーネント
//===========================================================================
#include "BossConfrontForward.h"
#include <Game/Component/Enemy/Boss/BossAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>

//! @brief 初期化
void BossConfrontForward::ConfrontInit()
{
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションを設定
        anim_control->SetFlag(BossAnimationController::STATE_BIT::WALK_FRONT);
    }
}

//! @brief 更新
void BossConfrontForward::ConfrontUpdate()
{
    auto player = GetPlayer();
    auto move   = GetOwner()->GetComponent<EnemyMove>();

    if(!player || !move)
    {
        return;
    }

    // プレイヤー方向への移動ベクトルを計算
    float3 dir = player->GetTranslate() - GetOwner()->GetTranslate();
    dir        = normalize(dir);
    move->SetVelocity(dir);

    // プレイヤーの方向を向く
    GetOwner()->SetRotationToPositionWithLimit(player->GetTranslate(), 3.0f);
}

//! @brief 終了
void BossConfrontForward::ConfrontExit()
{
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションをリセット
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::WALK_FRONT);
    }
}

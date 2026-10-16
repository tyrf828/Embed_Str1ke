//===========================================================================
//! @file BossConfrontStay.cpp
//! @brief ボスの停止コンポーネント
//===========================================================================
#include "BossConfrontStay.h"
#include <Game/Component/Enemy/Boss/BossAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>

//! @brief 初期化
void BossConfrontStay::ConfrontInit()
{
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションを設定
        anim_control->SetFlag(BossAnimationController::STATE_BIT::IDLE);
    }
}

//! @brief 更新
void BossConfrontStay::ConfrontUpdate()
{
    auto player = GetPlayer();
    auto move   = GetOwner()->GetComponent<EnemyMove>();

    if(!player || !move)
    {
        return;
    }

    // 停止
    move->SetVelocity({0.0f, 0.0f, 0.0f});

    // プレイヤーの方向を向く
    GetOwner()->SetRotationToPositionWithLimit(player->GetTranslate(), 1.0f);
}

//! @brief 終了
void BossConfrontStay::ConfrontExit()
{
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションをリセット
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::IDLE);
    }
}

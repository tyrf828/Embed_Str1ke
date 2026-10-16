//===========================================================================
//! @file BossWalk.cpp
//! @brief ボスの歩行コンポーネント
//===========================================================================
#include "BossWalk.h"
#include <Game/Component/Enemy/Boss/BossAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/Enemy/Boss/Action/BossAction.h>
#include <Game/Component/Enemy/Boss/Action/BossDashAttack.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
void BossWalk::Init()
{
    Super::Init();

    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションを設定
        anim_control->SetFlag(BossAnimationController::STATE_BIT::WALK_FRONT);
    }
}

//! @brief 更新
void BossWalk::Update()
{
    Super::Update();

    auto player = GetPlayer();
    auto move   = GetOwner()->GetComponent<EnemyMove>();

    if(!player || !move)
    {
        return;
    }

    // 衝突判定を削除
    RemoveSphereCollisions();

    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        // 音を再生
        se->PlayNoSame("walk");
    }

    // プレイヤーの方向を向く
    GetOwner()->SetRotationToPositionWithLimit(player->GetTranslate(), 5.0f);
    if(GetPlayerDegree() > 60.0f)
    {
        return;
    }

    // プレイヤーまでの距離を確認
    if(GetPlayerDistance() <= attack_distance_)
    {
        // 攻撃距離内に入ったら前のアクションに戻る
        if(auto e_action = GetOwner()->GetComponent<BossAction>())
        {
            auto prev_action = e_action->GetPrevAction();
            e_action->SetAction(prev_action);
        }
        return;
    }

    // プレイヤー方向への移動ベクトルを計算
    float3 dir = player->GetTranslate() - GetOwner()->GetTranslate();
    move->SetVelocity(-GetOwner()->GetVectorAxisZ());
    dash_attack_timer_ += GetDeltaTime();

    // 一定時間経過したらまだプレイヤとの距離がある場合は突進攻撃に移行
    if(dash_attack_timer_ >= DASH_ATTACK_TIME_)
    {
        dash_attack_timer_ = 0.0f;
        // 突進攻撃に移行
        if(auto e_action = GetOwner()->GetComponent<BossAction>())
        {
            e_action->SetAction(std::make_shared<BossDashAttack>());
        }
    }
}

//! @brief 終了
void BossWalk::Exit()
{
    Super::Exit();

    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションフラグをリセット
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::WALK_FRONT);
    }

    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動を停止
        move->SetVelocity({0.0f, 0.0f, 0.0f});
    }

    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        // 音を停止
        se->Stop("walk");
    }
}

//! @brief GUI表示処理
void BossWalk::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("BossWalk"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::Text(u8"プレイヤーまでの距離: %.2f", GetPlayerDistance());
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//===========================================================================
//! @file BossReady.cpp
//! @brief ボスの戦闘準備コンポーネント
//===========================================================================
#include "BossReady.h"
#include <Game/Component/Enemy/Boss/BossAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <System/Component/ComponentModel.h>
#include <Game/Component/Enemy/Boss/Action/BossAction.h>
#include <Game/Component/Enemy/Boss/Action/BossIdle.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/Player/Move.h>

//! @brief 初期化
void BossReady::Init()
{
    Super::Init();

    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションを設定
        anim_control->SetFlag(BossAnimationController::STATE_BIT::READY);
    }
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        move->SetVelocity({0.0f, 0.0f, 0.0f});
    }
    if(auto model = GetOwner()->GetComponent<ComponentModel>())
    {
        // モデルのXZ軸固定を解除
        model->SetOnPlaceXZ(false);
    }
    if(auto player = Scene::Object::Get<Player>())
    {
        player->SetActionEnable(false);
        if(auto player_move = player->GetComponent<Move>())
        {
            // プレイヤーの移動を無効化
            player_move->SetEnable(false);
        }
    }
}

//! @brief 更新
void BossReady::Update()
{
    Super::Update();
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        if(!anim_control->IsPlaying())
        {
            // アクションを待機に変更
            if(auto e_action = GetOwner()->GetComponent<BossAction>())
            {
                e_action->SetAction(std::make_shared<BossIdle>());
            }
        }
    }
}

//! @brief 終了
void BossReady::Exit()
{
    Super::Exit();
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションフラグをリセット
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::READY);
    }
    if(auto model = GetOwner()->GetComponent<ComponentModel>())
    {
        // モデルのXZ軸固定を有効化
        model->SetOnPlaceXZ(true);
    }
    if(auto player = Scene::Object::Get<Player>())
    {
        player->SetActionEnable(true);
        if(auto player_move = player->GetComponent<Move>())
        {
            // プレイヤーの移動を有効化
            player_move->SetEnable(true);
        }
    }
}

//! @brief GUI表示処理
void BossReady::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("BossReady"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

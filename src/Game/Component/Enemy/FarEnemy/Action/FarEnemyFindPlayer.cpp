//===========================================================================
//! @file FarEnemyFindPlayer.cpp
//! @brief 敵がプレイヤーを探すコンポーネント
//===========================================================================
#include "FarEnemyFindPlayer.h"
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/Enemy/FarEnemy/FarEnemyAnimationController.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyAction.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyThreat.h>
#include <Game/Component/Navmesh.h>
#include <System/Lib.h>
#include <Game/Object/Manager/SpawnManager.h>
#include <Game/Object/Manager/EnemyMentalManager.h>
#include <Game/Object/Enemy/EnemyBase.h>

//! @brief 初期化
void FarEnemyFindPlayer::Init()
{
    Super::Init();

    auto player = Scene::Object::Get<Player>();
    if(!player)
    {
        return;
    }
    auto move              = GetOwner()->GetComponent<EnemyMove>();
    auto mental_controller = Scene::Object::Get<EnemyMentalManager>();
    // ターゲットにプレイヤーを設定
    if(auto nav = GetOwner()->GetComponent<Navmesh>())
    {
        nav->UseNavmesh(true);
        if(mental_controller)
        {
            auto enemy_base = std::dynamic_pointer_cast<EnemyBase>(GetOwner());
            target_pos_     = mental_controller->GetRecordTargetPos(enemy_base);
            nav->SetTarget(target_pos_);
        }
        else
        {
            nav->SetTarget(player);
        }
    }
    else if(move)
    {
        move->SetTarget(player);
    }
    if(move)
    {
        move->SetSpeed(WALK_SPEED_);
    }
}

//! @brief 更新
void FarEnemyFindPlayer::Update()
{
    Super::Update();

    auto move         = GetOwner()->GetComponent<EnemyMove>();
    auto anim_control = GetOwner()->GetComponent<FarEnemyAnimationController>();
    if(!move || !anim_control)
    {
        return;
    }

    // 移動方向を向く
    if(length(move->GetVelocity()) > (float1)FLT_EPSILON)
    {
        float3 vector = move->GetVelocity();
        vector.y      = 0.0f;
        GetOwner()->SetRotationToVectorWithLimit(vector, 3.0f);
    }

    // アニメーション
    anim_control->SetFlag(FarEnemyAnimationController::STATE_BIT::WALK);

    float3 target_vec = target_pos_ - GetOwner()->GetTranslate();
    float distance    = sqrt(dot(target_vec, target_vec));

    // ランダムなオフセットを生成
    float random_offset = GetRandomF(-5.0f, 5.0f);

    // プレイヤーが一定距離内に入ったら威嚇アクションへ遷移
    if(GetPlayerDistance() <= FarEnemyThreat::GetThreatMaxDistanceStatic() - random_offset || distance < 5.0f)
    {
        if(auto e_action = GetOwner()->GetComponent<FarEnemyAction>())
        {
            e_action->SetAction(std::make_shared<FarEnemyThreat>());
        }
    }
}

void FarEnemyFindPlayer::LateDraw()
{
    Super::LateDraw();
}

//! @brief 終了
void FarEnemyFindPlayer::Exit()
{
    Super::Exit();

    // ターゲット解除
    if(auto nav = GetOwner()->GetComponent<Navmesh>())
    {
        nav->SetTarget(nullptr);
        nav->UseNavmesh(false);
    }
    else if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        move->SetTarget(nullptr);
    }

    if(auto anim_control = GetOwner()->GetComponent<FarEnemyAnimationController>())
    {
        // アニメーションフラグリセット
        anim_control->ResetFlag(FarEnemyAnimationController::STATE_BIT::WALK);
    }
}

//! @brief GUI表示処理
void FarEnemyFindPlayer::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("EnemyFindPlayer"))
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

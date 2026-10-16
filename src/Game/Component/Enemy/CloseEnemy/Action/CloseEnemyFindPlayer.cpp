//===========================================================================
//! @file CloseEnemyFindPlayer.cpp
//! @brief 敵がプレイヤーを探すコンポーネント
//===========================================================================
#include "CloseEnemyFindPlayer.h"
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/Enemy/CloseEnemy/CloseEnemyAnimationController.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyAction.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyThreat.h>
#include <Game/Component/Navmesh.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Object/Manager/SpawnManager.h>
#include <Game/Object/Manager/EnemyMentalManager.h>
#include <Game/Object/Enemy/EnemyBase.h>

//! @brief 初期化
void CloseEnemyFindPlayer::Init()
{
    Super::Init();
    auto player = Scene::Object::Get<Player>();
    if(!player)
    {
        return;
    }
    auto move              = GetOwner()->GetComponent<EnemyMove>();
    auto mental_controller = Scene::Object::Get<EnemyMentalManager>();

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
void CloseEnemyFindPlayer::Update()
{
    Super::Update();
    auto anim_control = GetOwner()->GetComponent<CloseEnemyAnimationController>();
    if(!anim_control)
    {
        return;
    }
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動方向を向く
        if(length(move->GetVelocity()) > (float1)FLT_EPSILON)
        {
            float3 vector = move->GetVelocity();
            vector.y      = 0.0f;
            GetOwner()->SetRotationToVectorWithLimit(vector, 3.0f);
        }
    }

    // アニメーション
    anim_control->SetFlag(CloseEnemyAnimationController::STATE_BIT::WALK);

    float3 target_vec = target_pos_ - GetOwner()->GetTranslate();
    float distance    = sqrt(dot(target_vec, target_vec));

    // プレイヤーが一定距離内に入ったら威嚇アクションへ遷移
    float diff   = CloseEnemyThreat::GetThreatMaxDistanceStatic() - CloseEnemyThreat::GetThreatMinDistanceStatic();
    float offset = CloseEnemyThreat::GetThreatMinDistanceStatic() + diff * 0.5f;
    if(GetPlayerDistance() <= offset || distance < 5.0f)
    {
        if(auto e_action = GetOwner()->GetComponent<CloseEnemyAction>())
        {
            e_action->SetAction(std::make_shared<CloseEnemyThreat>());
        }
    }
}

void CloseEnemyFindPlayer::LateDraw()
{
    Super::LateDraw();
#if 0
    DrawSphere3D(cast(target_pos_), 5.0f, 16, GetColor(255, 0, 0), GetColor(255, 0, 0), FALSE);
#endif
}

//! @brief 終了
void CloseEnemyFindPlayer::Exit()
{
    Super::Exit();

    if(auto nav = GetOwner()->GetComponent<Navmesh>())
    {
        nav->SetTarget(nullptr);
        nav->UseNavmesh(false);
    }
    else if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        move->SetTarget(nullptr);
    }
    if(auto anim_control = GetOwner()->GetComponent<CloseEnemyAnimationController>())
    {
        anim_control->ResetFlag(CloseEnemyAnimationController::STATE_BIT::WALK);
    }
}

//! @brief GUI表示処理
void CloseEnemyFindPlayer::GUI()
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

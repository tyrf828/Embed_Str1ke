//===========================================================================
//! @file CloseEnemyAttack.cpp
//! @brief 敵の攻撃コンポーネント
//===========================================================================
#include "CloseEnemyAttack.h"
#include <System/Component/ComponentModel.h>
#include <Game/Component/Enemy/CloseEnemy/CloseEnemyAnimationController.h>
#include "Game/Object/Player/Player.h"
#include "Game/Component/Enemy/CloseEnemy/Action/CloseEnemyAction.h"
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/Sound.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyThreat.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyFindPlayer.h>
#include <Game/Component/Enemy/AttackParam.h>
#include <Game/Object/Enemy/CloseEnemy/CloseEnemy.h>
#include <Game/Component/ReciprocalVelocityObstacles.h>
#include <Game/Component/Navmesh.h>

//! @brief 初期化
void CloseEnemyAttack::Init()
{
    Super::Init();

    // 攻撃力を設定
    if(auto atk = GetOwner()->GetComponent<AttackParam>())
    {
        atk->SetAttack(ATK_);
    }

    // 攻撃判定用のコリジョンを追加
    if(auto owner = dynamic_cast<CloseEnemy*>(GetOwner().get()))
    {
        if(auto weapon = owner->GetWeaponPtr())
        {
            weapon->AddCollision();
        }
    }
    if(auto navmesh = GetOwner()->GetComponent<Navmesh>())
    {
        navmesh->UseNavmesh(true);
    }
    is_attack_ = false;
}

//! @brief 更新
void CloseEnemyAttack::Update()
{
    Super::Update();

    // モデルコンポーネントの取得
    auto model        = GetOwner()->GetComponent<ComponentModel>();
    auto anim_control = GetOwner()->GetComponent<CloseEnemyAnimationController>();
    auto owner        = dynamic_cast<CloseEnemy*>(GetOwner().get());
    auto navmesh      = GetOwner()->GetComponent<Navmesh>();
    auto player       = Scene::Object::Get<Player>();
    auto move         = GetOwner()->GetComponent<EnemyMove>();
    if(!model || !anim_control || !navmesh || !player || !move)
    {
        return;
    }

    // プレイヤーが攻撃距離以内
    if(navmesh->GetNavmeshDistance(player->GetTranslate()) < ATTACK_DISTANCE_)
    {
        // 攻撃
        is_attack_ = true;

        navmesh->SetTarget(nullptr);
        navmesh->UseNavmesh(false);
    }
    // 攻撃ではない時
    if(!is_attack_)
    {
        if(navmesh)
        {
            if(!navmesh->GetTarget())
            {
                navmesh->SetTarget(player);
            }
        }
        else
        {
            // 移動を有効化
            move->SetMove(true);
            move->SetTarget(Scene::Object::Get<Player>());
        }
        // アニメーション再生
        anim_control->SetFlag(CloseEnemyAnimationController::WALK);

        if(auto se = GetOwner()->GetComponent<Sound>())
        {
            // 音を再生
            se->PlayNoSame("walk");
        }
        // 前の方向を向く
        if(length((float1)move->GetVelocityLength()) > (float1)FLT_EPSILON)
        {
            GetOwner()->SetRotationToVectorWithLimit(move->GetVelocity(), 1.5f);
        }
    }
    else
    {
        if(auto se = GetOwner()->GetComponent<Sound>())
        {
            // 音を再生
            se->Stop("walk");
        }

        // アニメーション再生
        anim_control->SetFlag(CloseEnemyAnimationController::STATE_BIT::ATTACK);
        // 攻撃判定の処理
        if(anim_control->IsAnimationTimeInRange(ATTACK_START_TIME_, ATTACK_END_TIME_))
        {
            if(owner->GetWeaponPtr())
            {
                owner->GetWeaponPtr()->SetHitEnable(true);
            }
        }
        else
        {
            // 当たり判定を変更
            if(owner->GetWeaponPtr())
            {
                owner->GetWeaponPtr()->SetHitEnable(false);
            }
        }
        // プレイヤーの方向を向く
        GetOwner()->SetRotationToPositionWithLimit(player->GetTranslate(), 2.0f);

        // 攻撃終了後の処理
        if(!model->IsPlaying())
        {
            if(auto e_action = GetOwner()->GetComponent<CloseEnemyAction>())
            {
                e_action->SetAction(std::make_shared<CloseEnemyThreat>());
            }
        }
    }
}

//! @brief 終了
void CloseEnemyAttack::Exit()
{
    Super::Exit();
    // 攻撃判定用のコリジョンを削除
    if(auto owner = dynamic_cast<CloseEnemy*>(GetOwner().get()))
    {
        if(auto weapon = owner->GetWeaponPtr())
        {
            weapon->RemoveCollision();
        }
    }
    if(auto navmesh = GetOwner()->GetComponent<Navmesh>())
    {
        navmesh->SetTarget(nullptr);
        navmesh->UseNavmesh(false);
    }
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動を無効化
        move->SetMove(true);
        move->SetTarget(nullptr);
    }
    if(auto anim_control = GetOwner()->GetComponent<CloseEnemyAnimationController>())
    {
        anim_control->ResetFlag(CloseEnemyAnimationController::WALK);
        anim_control->ResetFlag(CloseEnemyAnimationController::STATE_BIT::ATTACK);
    }
    if(auto atk = GetOwner()->GetComponent<AttackParam>())
    {
        atk->SetAttack(0);
    }
    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        // 音を再生
        se->Stop("walk");
    }
}

//! @brief GUI表示処理
void CloseEnemyAttack::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("EnemyAttack"))
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

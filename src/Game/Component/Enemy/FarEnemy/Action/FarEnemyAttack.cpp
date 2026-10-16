//===========================================================================
//! @file FarEnemyAttack.cpp
//! @brief 敵の攻撃コンポーネント
//===========================================================================
#include "FarEnemyAttack.h"
#include <System/Component/ComponentModel.h>
#include <Game/Component/Enemy/FarEnemy/FarEnemyAnimationController.h>
#include "Game/Object/Player/Player.h"
#include <System/Component/ComponentCollisionSphere.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/Sound.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyThreat.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyIdle.h>
#include <Game/Component/Enemy/FarEnemy/Action/FarEnemyFindPlayer.h>
#include "Game/Component/Enemy/FarEnemy/Action/FarEnemyAction.h"
#include <Game/Component/Enemy/AttackParam.h>
#include <Game/Object/Enemy/FarEnemy/FarEnemy.h>
#include <Game/Component/Navmesh.h>

//! @brief 初期化
void FarEnemyAttack::Init()
{
    Super::Init();

    if(auto atk = GetOwner()->GetComponent<AttackParam>())
    {
        // 攻撃力を設定
        atk->SetAttack(ATK_);
    }

    if(auto navmesh = GetOwner()->GetComponent<Navmesh>())
    {
        navmesh->UseNavmesh(true);
    }
    is_attack_ = false;
}

//! @brief 更新
void FarEnemyAttack::Update()
{
    Super::Update();

    auto model        = GetOwner()->GetComponent<ComponentModel>();
    auto anim_control = GetOwner()->GetComponent<FarEnemyAnimationController>();
    auto owner        = dynamic_cast<FarEnemy*>(GetOwner().get());
    auto navmesh      = GetOwner()->GetComponent<Navmesh>();
    auto player       = Scene::Object::Get<Player>();
    auto move         = GetOwner()->GetComponent<EnemyMove>();
    if(!model || !anim_control || !navmesh || !player || !move)
    {
        return;
    }

    // 視線チェック
    is_player_visible_ = false;
    MV1_COLL_RESULT_POLY hit_poly{};
    if(!is_attack_)
    {
        if(auto stage = Scene::Object::Get<Object>("Stage"))
        {
            if(auto stage_model = stage->GetComponent<ComponentModel>())
            {
                float3 enemy_pos  = GetOwner()->GetTranslate() + float3{0.0f, 15.0f, 0.0f};
                float3 player_pos = player->GetTranslate() + float3{0.0f, 15.0f, 0.0f};
                hit_poly          = MV1CollCheck_Line(stage_model->GetModel(), -1, cast(enemy_pos), cast(player_pos));
            }
        }
        // プレイヤーが見えるかチェック
        is_player_visible_ = !hit_poly.HitFlag;
    }

    // プレイヤーまでの距離を取得
    float player_distance = navmesh->GetNavmeshDistance(player->GetTranslate());

    // プレイヤーが見えて攻撃距離以内
    if(is_player_visible_ && player_distance < ATTACK_DISTANCE_)
    {
        is_attack_ = true;
        // 攻撃中の処理
        if(auto se = GetOwner()->GetComponent<Sound>())
        {
            se->Stop("walk");
        }

        // 攻撃アニメーション再生
        anim_control->SetFlag(FarEnemyAnimationController::ATTACK);

        navmesh->SetTarget(nullptr);
        navmesh->UseNavmesh(false);

        // 攻撃判定を有効化
        owner->GetWeaponPtr()->SetHit();

        // 射撃時にプレイヤに向いていない可能性があるから、ちょっとだけ一瞬上げる
        GetOwner()->SetRotationToPositionWithLimit(player->GetTranslate(), 5.0f);
    }

    // 攻撃ではない時
    if(!is_attack_)
    {
        // プレイヤーが見えないか攻撃距離外で、まだ一度も移動していない場合
        if((!is_player_visible_ || player_distance >= ATTACK_DISTANCE_))
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
                move->SetMove(true);
                move->SetTarget(player);
            }

            anim_control->SetFlag(FarEnemyAnimationController::WALK);

            if(auto se = GetOwner()->GetComponent<Sound>())
            {
                se->PlayNoSame("walk");
            }

            if(length((float1)move->GetVelocityLength()) > (float1)FLT_EPSILON)
            {
                GetOwner()->SetRotationToVectorWithLimit(move->GetVelocity(), 2.0f);
            }
        }
        else
        {
            // 移動を停止
            if(navmesh)
            {
                navmesh->SetTarget(nullptr);
            }
            move->SetMove(false);

            if(auto se = GetOwner()->GetComponent<Sound>())
            {
                se->Stop("walk");
            }

            anim_control->ResetFlag(FarEnemyAnimationController::WALK);
        }
    }
    else
    {
        // プレイヤーの方向を向く
        if(model->IsPlaying())
        {
            const float TRACK_SPEED = 0.3f;
            owner->SetRotationToPositionWithLimit(player->GetTranslate(), TRACK_SPEED);
        }

        // 攻撃終了後の処理
        if(!model->IsPlaying() && !owner->GetWeaponPtr()->GetEffect())
        {
            if(auto e_action = GetOwner()->GetComponent<FarEnemyAction>())
            {
                e_action->SetAction(std::make_shared<FarEnemyThreat>());
            }
        }
    }
}

//! @brief 終了
void FarEnemyAttack::Exit()
{
    Super::Exit();
    if(auto navmesh = GetOwner()->GetComponent<Navmesh>())
    {
        navmesh->SetTarget(nullptr);
        navmesh->UseNavmesh(false);
    }
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        move->SetMove(true);
        move->SetTarget(nullptr);
    }
    if(auto anim_control = GetOwner()->GetComponent<FarEnemyAnimationController>())
    {
        anim_control->ResetFlag(FarEnemyAnimationController::WALK);
        anim_control->ResetFlag(FarEnemyAnimationController::STATE_BIT::ATTACK);
    }
    if(auto atk = GetOwner()->GetComponent<AttackParam>())
    {
        atk->SetAttack(0);
    }
    if(auto owner = dynamic_cast<FarEnemy*>(GetOwner().get()))
    {
        owner->GetWeaponPtr()->RemoveHit();
    }
    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        se->Stop("walk");
    }
}

//! @brief GUI表示処理
void FarEnemyAttack::GUI()
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

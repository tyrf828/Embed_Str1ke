//===========================================================================
//! @file BossKick.cpp
//! @brief ボスのキック攻撃コンポーネント
//===========================================================================
#include "BossKick.h"
#include <Game/Component/Enemy/Boss/BossAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <Game/Component/Enemy/Boss/Action/BossAction.h>
#include <Game/Component/Enemy/Boss/Action/BossIdle.h>
#include <Game/Component/Enemy/Boss/Action/BossWalk.h>
#include <Game/Component/Enemy/Boss/BossActionManager.h>
#include <Game/Component/Enemy/AttackParam.h>
#include <Game/Object/Effect/BossHitEffect.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
void BossKick::Init()
{
    Super::Init();

    if(auto e_action = GetOwner()->GetComponent<BossAction>())
    {
        // 攻撃距離外なら一旦歩行アクションに切り替え
        if(GetPlayerDistance() > ATTACK_DISTANCE_)
        {
            auto walk = std::make_shared<BossWalk>();
            walk->SetAttackDistance(ATTACK_DISTANCE_);
            e_action->SetAction(walk);
            return;
        }
    }

    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // キックアニメーションを設定
        anim_control->SetFlag(BossAnimationController::STATE_BIT::KICK);
    }

    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        move->SetVelocity({0.0f, 0.0f, 0.0f});
    }

    // 攻撃判定用の球体コリジョンを追加
    AddSphereCollision("mixamorig:RightFoot", 7.5f);

    if(auto atk = GetOwner()->GetComponent<AttackParam>())
    {
        // 攻撃力を設定
        atk->SetAttack(ATTACK_POINT_);
    }
    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        // 音を再生
        se->Play("attack sound1");
    }
}

//! @brief 更新
void BossKick::Update()
{
    Super::Update();
    auto anim     = GetOwner()->GetComponent<BossAnimationController>();
    auto e_action = GetOwner()->GetComponent<BossAction>();
    auto col      = GetOwner()->GetComponent<ComponentCollisionSphere>();
    if(!anim || !e_action || !col)
    {
        return;
    }

    // ターゲット方向へのベクトル計算
    if(auto player = GetPlayer())
    {
        GetOwner()->SetRotationToPositionWithLimit(player->GetTranslate(), 3.0f);
    }

    // 攻撃判定の有効化
    if(anim->IsAnimationTimeInRange(ATTACK_START_TIME_, ATTACK_END_TIME_))
    {
        col->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY_WEAPON);
    }
    else
    {
        col->SetCollisionGroup(ComponentCollision::CollisionGroup::ETC);
    }

    // アニメーション終了時にアイドル状態へ遷移
    if(!anim->IsPlaying())
    {
        e_action->SetAction(std::make_shared<BossIdle>());
    }
}

//! @brief 衝突時
//! @param hit_info 衝突情報
void BossKick::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    auto sphere_col = std::dynamic_pointer_cast<ComponentCollisionSphere>(hit_info.collision_);
    if(!sphere_col)
    {
        return;
    }
    // プレイヤーとの衝突を検出
    if(static_cast<u32>(hit_info.hit_collision_->GetCollisionGroup()) & static_cast<u32>(ComponentCollision::CollisionGroup::PLAYER))
    {
        attack_hit_ = true;
        // ヒットエフェクトの生成
        if(!hit_effect_)
        {
            hit_effect_ = Scene::Object::Create<BossHitEffect>();
            hit_effect_->SetTranslate(hit_info.hit_position_);
        }
        // 音を再生
        if(auto se = GetOwner()->GetComponent<Sound>())
        {
            se->Play("attack success");
        }
    }
    Super::OnHit(hit_info);
}

//! @brief 終了
void BossKick::Exit()
{
    Super::Exit();
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションフラグのリセット
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::KICK);
    }

    // 攻撃判定用コリジョンの削除
    RemoveSphereCollisions();

    // ヒットエフェクトの解放
    if(hit_effect_)
    {
        Scene::Object::Release(hit_effect_);
        hit_effect_.reset();
    }

    // 攻撃終了時に採点
    if(auto action_manager = GetOwner()->GetComponent<BossActionManager>())
    {
        if(attack_hit_)
        {
            action_manager->OnAttackSuccess(BossActionManager::AttackType::KICK);
        }
        else
        {
            action_manager->OnAttackFailed(BossActionManager::AttackType::KICK);
        }
    }
}

//! @brief GUI表示処理
void BossKick::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("BossKick"))
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

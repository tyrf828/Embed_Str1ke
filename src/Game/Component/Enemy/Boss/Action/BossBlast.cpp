//===========================================================================
//! @file BossBlast.cpp
//! @brief ボスの爆発攻撃コンポーネント
//===========================================================================
#include "BossBlast.h"
#include <Game/Component/Enemy/Boss/BossAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/Enemy/Boss/Action/BossAction.h>
#include <Game/Component/Enemy/Boss/Action/BossIdle.h>
#include <Game/Component/Enemy/Boss/Action/BossWalk.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <Game/Component/Enemy/Boss/BossActionManager.h>
#include <Game/Component/Enemy/AttackParam.h>
#include <Game/Object/Effect/BossBlastEffect.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
void BossBlast::Init()
{
    Super::Init();

    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションを設定
        anim_control->SetFlag(BossAnimationController::STATE_BIT::BLAST);
    }
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動を停止
        move->SetVelocity({0.0f, 0.0f, 0.0f});
    }

    // 攻撃判定用の球体コリジョンを追加
    AddSphereCollision("mixamorig:Spine", ATTACK_RADIUS_);
    if(auto e_action = GetOwner()->GetComponent<BossAction>())
    {
        // プレイヤーとの距離が遠い場合は一旦歩行アクションに切り替え
        if(GetPlayerDistance() > ATTACK_RADIUS_)
        {
            auto walk = std::make_shared<BossWalk>();
            walk->SetAttackDistance(ATTACK_RADIUS_ * 0.8f);
            e_action->SetAction(walk);
        }
    }
    if(auto atk = GetOwner()->GetComponent<AttackParam>())
    {
        // 攻撃力を設定
        atk->SetAttack(ATTACK_POINT_);
    }
}

//! @brief 更新
void BossBlast::Update()
{
    Super::Update();
    auto anim     = GetOwner()->GetComponent<BossAnimationController>();
    auto e_action = GetOwner()->GetComponent<BossAction>();
    auto col      = GetOwner()->GetComponent<ComponentCollisionSphere>();
    if(!anim || !e_action)
    {
        return;
    }

    // アニメーション終了時にアイドル状態へ遷移
    if(!anim->IsPlaying())
    {
        e_action->SetAction(std::make_shared<BossIdle>());
    }
    // 攻撃判定の有効期間中
    if(anim->IsAnimationTimeInRange(ATTACK_START_TIME_, ATTACK_END_TIME_))
    {
        col->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY_WEAPON2);
        // エフェクト生成
        if(!blast_effect_)
        {
            blast_effect_ = Scene::Object::Create<BossBlastEffect>();
            float3 offset = float3{0.0f, 1.0f, 0.0f};
            blast_effect_->SetTranslate(GetOwner()->GetTranslate() + offset);
        }
        // 効果音再生
        if(auto se = GetOwner()->GetComponent<Sound>())
        {
            se->PlayOnce("blast");
        }
    }
    else
    {
        // 攻撃判定無効化
        col->SetCollisionGroup(ComponentCollision::CollisionGroup::ETC);
    }
}

//! @brief 衝突時
//! @param hit_info 衝突情報
void BossBlast::OnHit(const ComponentCollision::HitInfo& hit_info)
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
    }
    Super::OnHit(hit_info);
}

//! @brief 終了
void BossBlast::Exit()
{
    Super::Exit();
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションフラグのリセット
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::BLAST);
    }

    // 攻撃判定用コリジョンの削除
    RemoveSphereCollisions();

    // 攻撃終了時に採点
    if(auto action_manager = GetOwner()->GetComponent<BossActionManager>())
    {
        if(attack_hit_)
        {
            action_manager->OnAttackSuccess(BossActionManager::AttackType::BLAST);
        }
        else
        {
            action_manager->OnAttackFailed(BossActionManager::AttackType::BLAST);
        }
    }
    // エフェクトの解放
    if(blast_effect_)
    {
        Scene::Object::Release(blast_effect_);
        blast_effect_.reset();
    }
    // 音の停止
    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        se->Stop("blast");
    }
}

//! @brief GUI表示処理
void BossBlast::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("BossBlast"))
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

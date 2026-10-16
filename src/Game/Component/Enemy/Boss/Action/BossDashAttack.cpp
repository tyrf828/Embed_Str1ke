//===========================================================================
//! @file BossDashAttack.cpp
//! @brief ボスの突進攻撃コンポーネント
//===========================================================================
#include "BossDashAttack.h"
#include <Game/Component/Enemy/Boss/BossAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentModel.h>
#include <Game/Component/Enemy/Boss/Action/BossAction.h>
#include <Game/Component/Enemy/Boss/Action/BossIdle.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <Game/Component/Enemy/Boss/BossActionManager.h>
#include <Game/Component/Enemy/AttackParam.h>
#include <Game/Object/Effect/BossHitEffect.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
void BossDashAttack::Init()
{
    Super::Init();

    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションを設定
        anim_control->SetFlag(BossAnimationController::STATE_BIT::DASH_ATTACK);
    }
    // 開始位置と目標位置を記録
    init_pos_   = GetOwner()->GetTranslate();
    target_pos_ = GetPlayer()->GetTranslate();
    if(auto col = GetOwner()->GetComponent<ComponentCollisionCapsule>())
    {
        // 重力を無効化
        col->UseGravity(false);
    }

    // 攻撃判定用の球体コリジョンを追加
    AddSphereCollision("mixamorig:RightHand", 7.5f);
    if(auto atk = GetOwner()->GetComponent<AttackParam>())
    {
        // 攻撃力を設定
        atk->SetAttack(ATTACK_POINT_);
    }
    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        // 音を再生
        se->PlayNoSame("dash sound");
    }
}

//! @brief 更新
void BossDashAttack::Update()
{
    Super::Update();
    auto anim  = GetOwner()->GetComponent<BossAnimationController>();
    auto model = GetOwner()->GetComponent<ComponentModel>();
    auto col   = GetOwner()->GetComponent<ComponentCollisionSphere>();
    if(!anim || !model || !col)
    {
        return;
    }
    if(!anim->GetNowPlaying((int)BossAnimationController::STATE::DASH_ATTACK))
    {
        return;
    }
    // ターゲット方向へのベクトル計算
    if(auto player = GetPlayer())
    {
        GetOwner()->SetRotationToPositionWithLimit(player->GetTranslate(), 5.0f);
    }

    float3 vec = target_pos_ - init_pos_;
    vec.y      = 0;
    vec        = normalize(vec);
    // 突進移動処理
    time_             += GetDeltaTime() * 2;
    time_              = std::min(time_, 1.0f);
    float smooth_time  = smoothstep((float1)0.0f, (float1)1.0f, time_);
    if(time_ < 1.0f)
    {
        target_pos_          = GetPlayer()->GetTranslate();
        float3 offset_target = target_pos_ - (normalize(vec) * 10);
        float3 translate     = lerp(init_pos_, offset_target, smooth_time);
        GetOwner()->SetTranslate(translate + float3{0, 10 * smooth_time, 0});
    }

    // スローモーション演出
    if(anim->IsAnimationTimeInRange(1.34f) && slow_elapsed_time_ <= 0.0f)
    {
        slow_motion_ = true;
        model->SetTimeScale(0.1f);
    }

    if(slow_motion_)
    {
        slow_elapsed_time_ += GetDeltaTime();
        // スローモーション解除
        if(slow_elapsed_time_ >= 0.3f)
        {
            slow_motion_ = false;
            model->SetTimeScale(1.0f);
        }
    }

    // 重力を有効化
    if(anim->IsAnimationTimeInRange(1.86f))
    {
        if(auto body_col = GetOwner()->GetComponent<ComponentCollisionCapsule>())
        {
            body_col->UseGravity(true);
        }
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

    // 音再生
    if(anim->IsAnimationTimeInRange(ATTACK_END_TIME_))
    {
        if(auto se = GetOwner()->GetComponent<Sound>())
        {
            se->PlayOnce("dash attack");
        }
    }

    // アニメーション終了時にアイドル状態へ遷移
    if(!model->IsPlaying())
    {
        if(auto e_action = GetOwner()->GetComponent<BossAction>())
        {
            e_action->SetAction(std::make_shared<BossIdle>());
        }
    }
}

//! @brief 衝突時
//! @param hit_info 衝突情報
void BossDashAttack::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    auto sphere_col = std::dynamic_pointer_cast<ComponentCollisionSphere>(hit_info.collision_);
    if(!sphere_col)
    {
        return;
    }

    // プレイヤーとの衝突を検出
    if(static_cast<u32>(hit_info.hit_collision_->GetCollisionGroup()) & static_cast<u32>(ComponentCollision::CollisionGroup::PLAYER))
    {
        // 攻撃がヒットしたことを記録
        attack_hit_ = true;
        // ヒットエフェクトの生成
        if(!hit_effect_)
        {
            hit_effect_ = Scene::Object::Create<BossHitEffect>();
            hit_effect_->SetTranslate(hit_info.hit_position_);
        }
    }
    Super::OnHit(hit_info);
}

//! @brief 終了
void BossDashAttack::Exit()
{
    Super::Exit();
    // アニメーションフラグのリセット
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::DASH_ATTACK);
        anim_control->SetTimeScale(1.0f);
    }
    // 重力を有効化
    if(auto col = GetOwner()->GetComponent<ComponentCollisionCapsule>())
    {
        col->UseGravity(true);
    }

    // 攻撃判定用コリジョンの削除
    RemoveSphereCollisions();

    auto action_manager = GetOwner()->GetComponent<BossActionManager>();
    if(!action_manager)
    {
        return;
    }

    // スコア調整用ラムダ
    auto adjust_score = [&](BossActionManager::AttackType type) {
        if(action_manager->GetAttackType() == type)
        {
            // 攻撃の成功かどうか採点
            if(attack_hit_)
            {
                action_manager->OnAttackSuccess(type);
            }
            else
            {
                action_manager->OnAttackFailed(type);
            }
        }
    };

    // 攻撃終了時に採点
    adjust_score(BossActionManager::AttackType::DASH);
    adjust_score(BossActionManager::AttackType::FALLBACK_DASH);

    // 音の停止
    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        se->Stop("dash attack");
    }

    // ヒットエフェクトの解放
    if(hit_effect_)
    {
        Scene::Object::Release(hit_effect_);
        hit_effect_.reset();
    }
}

//! @brief GUI表示処理
void BossDashAttack::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("BossDashAttack"))
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

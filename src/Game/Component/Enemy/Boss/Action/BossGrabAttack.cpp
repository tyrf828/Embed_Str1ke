//===========================================================================
//! @file BossGrabAttack.cpp
//! @brief ボスの掴み攻撃コンポーネント
//===========================================================================
#include "BossGrabAttack.h"
#include <Game/Component/Enemy/Boss/BossAnimationController.h>
#include <System/Component/ComponentModel.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/Enemy/Boss/Action/BossAction.h>
#include <Game/Component/Enemy/Boss/Action/BossIdle.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/Enemy/Boss/BossActionManager.h>
#include <Game/Component/Enemy/AttackParam.h>
#include <Game/Object/Effect/BossHitEffect.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
void BossGrabAttack::Init()
{
    Super::Init();

    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動を停止
        move->SetVelocity(float3{0.0f, 0.0f, 0.0f});
    }

    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションを設定
        anim_control->SetFlag(BossAnimationController::STATE_BIT::ATTACK);
    }

    // 初期位置とプレイヤの位置を記録
    init_pos_   = GetOwner()->GetTranslate();
    target_pos_ = GetPlayer()->GetTranslate();

    // 攻撃判定用の球体コリジョンを追加
    AddSphereCollision("mixamorig:RightHand", 5.0f);
    if(auto atk = GetOwner()->GetComponent<AttackParam>())
    {
        // 攻撃力を設定
        atk->SetAttack(ATTACK_POINT_);
    }
    attack_hit_ = false;
    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        // 攻撃音を再生
        se->Play("attack sound2");
    }
}

//! @brief 更新
void BossGrabAttack::Update()
{
    Super::Update();
    auto anim  = GetOwner()->GetComponent<BossAnimationController>();
    auto model = GetOwner()->GetComponent<ComponentModel>();
    auto col   = GetOwner()->GetComponent<ComponentCollisionSphere>();
    if(!anim || !model || !col)
    {
        return;
    }
    if(!anim->GetNowPlaying((int)BossAnimationController::STATE::ATTACK))
    {
        return;
    }

    // ターゲット方向へのベクトル計算
    float3 vec = target_pos_ - init_pos_;
    vec.y      = 0;
    GetOwner()->SetRotationToVectorWithLimit(vec, 5.0f);
    time_             += GetDeltaTime() * 2;
    time_              = std::min(time_, 1.0f);
    float smooth_time  = smoothstep((float1)0.0f, (float1)1.0f, time_);
    if(time_ < 1.0f)
    {
        float distance = length(vec);
        float factor   = std::min(1.0f, max_distance_ / distance);

        float3 clamped     = vec * factor;                   // 半径内に収める
        float final_length = length(clamped);                // 実距離
        float back_offset  = std::min(final_length, 5.0f);   // 手前オフセット

        float3 dir       = normalize(vec);   // 方向ベクトル
        float3 final_pos = init_pos_ + dir * (final_length - back_offset);
        float3 translate = lerp(init_pos_, final_pos, smooth_time);
        GetOwner()->SetTranslate(translate);
    }

    // アニメーション終了時にアイドル状態へ遷移
    if(anim->IsAnimationTimeInRange(ANIMATION_END_TIME_))
    {
        if(auto e_action = GetOwner()->GetComponent<BossAction>())
        {
            e_action->SetAction(std::make_shared<BossIdle>());
        }
    }
    // 攻撃判定の有効化
    if(anim->IsAnimationTimeInRange(ATTACK_START_TIME_, ATTACK_END_TIME_))
    {
        col->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY_WEAPON2);
    }
    else
    {
        col->SetCollisionGroup(ComponentCollision::CollisionGroup::ETC);
    }
    // 一定時間までスローモーション演出
    if(model->GetAnimationTime() < SLOW_TIME_ && anim->IsPlaying())
    {
        model->SetTimeScale(0.5f);
    }
    else
    {
        model->SetTimeScale(1.0f);
    }
}

//! @brief 衝突時
//! @param hit_info 衝突情報
void BossGrabAttack::OnHit(const ComponentCollision::HitInfo& hit_info)
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
void BossGrabAttack::Exit()
{
    Super::Exit();
    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションフラグをリセット
        anim_control->ResetFlag(BossAnimationController::STATE_BIT::ATTACK);
    }
    if(auto model = GetOwner()->GetComponent<ComponentModel>())
    {
        // タイムスケールを元に戻す
        model->SetTimeScale(1.0f);
    }

    // 攻撃判定用の球体コリジョンを削除
    RemoveSphereCollisions();

    // 攻撃終了時に採点
    if(auto action_manager = GetOwner()->GetComponent<BossActionManager>())
    {
        if(attack_hit_)
        {
            action_manager->OnAttackSuccess(BossActionManager::AttackType::GRAB);
        }
        else
        {
            action_manager->OnAttackFailed(BossActionManager::AttackType::GRAB);
        }
    }
}

//! @brief GUI表示処理
void BossGrabAttack::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("BossGrabAttack"))
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

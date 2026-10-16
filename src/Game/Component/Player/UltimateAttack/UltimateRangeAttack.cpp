//===========================================================================
//! @file UltimateRangeAttack.cpp
//! @brief 必殺技範囲攻撃
//===========================================================================
#include "UltimateRangeAttack.h"
#include <Game/Component/Player/UltimateAttack/UltimateAttack.h>
#include <Game/Object/Effect/UltimateAttackEffect.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <Game/Component/Player/UltimateAttack/UltimateThunder.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Component/Player/AnimationController.h>
#include <System/Lib.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
void UltimateRangeAttack::Init()
{
    Super::Init();

    auto ult_col = Scene::Object::Create<UltimateAttackCollision>("UltimateAttackCollision")->SetTranslate(ultimate_attack_->GetAttackPos());

    if(auto col = ult_col->GetComponent<ComponentCollisionSphere>())
    {
        col->SetRadius(ultimate_attack_->GetRadius());
    }
    if(auto anim = GetOwner()->GetComponent<AnimationController>())
    {
        // 走るアニメーションをリセット
        anim->ResetAllFlags();
    }
}

//! @brief 更新
void UltimateRangeAttack::Update()
{
    Super::Update();

    effect_spawn_elapsed_time_ += GetDeltaTime();

    if(effect_spawn_elapsed_time_ >= SPAWN_TIME_)
    {
        if(effect_num_ < EFFECT_MAX_)
        {
            effect_spawn_elapsed_time_ = 0.0f;

            // エフェクトを生成
            auto ult_effect = Scene::Object::Create<UltimateAttackEffect>("UltimateAttackEffect");

            const float rand_rot = GetRandomF(360.0f);
            const float r        = ultimate_attack_->GetRadius();
            ult_effect->SetTranslate(ultimate_attack_->GetAttackPos() + ult_effect->GetPlaneDir(rand_rot) * GetRandomF(r));
            ult_effect->SetRotationToPosition(ultimate_attack_->GetAttackPos() + float3(GetRandomF(r * 2) - r, r, GetRandomF(r * 2) - r), true, true);
            if(auto col = Scene::Object::Get<UltimateAttackCollision>()->GetComponent<ComponentCollisionSphere>())
            {
                col->SetCollisionGroup(ComponentCollision::CollisionGroup::ULTIMATE_ATTACK);
            }

            effect_num_++;
        }
        else
        {
            next_state_time += GetDeltaTime();
            if(next_state_time > 0.1f)
            {
                // 雷攻撃に移行する
                ultimate_attack_->SetAction(std::make_shared<UltimateThunder>());
            }
        }
    }
    else if(effect_spawn_elapsed_time_ > SPAWN_TIME_ * 0.5f)
    {
        if(auto col = Scene::Object::Get<UltimateAttackCollision>()->GetComponent<ComponentCollisionSphere>())
        {
            col->SetCollisionGroup(ComponentCollision::CollisionGroup::ETC);
        }
        if(auto se = GetOwner()->GetComponent<Sound>())
        {
            // 音を再生
            se->PlayNoSame("ultimate range");
        }
    }
}

//! @brief 描画
void UltimateRangeAttack::LateDraw()
{
    Super::LateDraw();
}

//! @brief 終了
void UltimateRangeAttack::Exit()
{
    Super::Exit();
    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        // 音を再生
        se->Stop("ultimate range");
    }
}

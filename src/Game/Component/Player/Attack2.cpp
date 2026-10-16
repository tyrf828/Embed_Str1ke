//===========================================================================
//! @file Attack2.cpp
//! @brief プレイヤーの攻撃コンポーネント
//===========================================================================
#include "Attack2.h"
#include "Attack3.h"
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Object/Weapon/Stick.h>
#include <Game/Component/Player/AnimationController.h>
#include <Game/Component/Player/Stamina.h>
#include <Game/Component/Player/Move.h>
#include <Game/Component/WeaponAttack.h>
#include <Game/Component/Sound.h>
#include <Game/Object/Player/Player.h>
#include <System/Component/ComponentModel.h>
#include <Game/Object/Player/Camera.h>

//! @brief 初期化
void Attack2::Init()
{
    Super::Init();

    // 初期化
    int random_sound       = GetRand(2) + 1;
    std::string sound_name = "attack" + std::to_string(random_sound);
    AttackBase::Initialize(AnimationController::STATE_BIT::ATTACK2, ATTACK_POINT_, sound_name);
    if(auto move = GetOwner()->GetComponent<Move>())
    {
        input_dir_ = move->GetVelDir();
    }
}

//! @brief 更新
void Attack2::Update()
{
    Super::Update();

    // 移動を無効化
    if(auto ctrl = GetOwner()->GetComponent<Move>())
    {
        ctrl->SetEnable(false);

        if(auto model = GetOwner()->GetComponent<ComponentModel>())
        {
            auto camera = Scene::Object::Get<Camera>();
            if(dot(input_dir_, input_dir_) > (float1)0.0f && !camera->IsCameraLock())
            {
                model->SetRotationToVectorWithLimit(input_dir_, 10.0f);
            }
        }
    }

    // コンポーネントを取得
    auto anim = GetOwner()->GetComponent<AnimationController>();
    if(!anim)
    {
        return;
    }

    // 攻撃が終わた場合
    // 攻撃を受けた場合
    if(anim->GetFlag(AnimationController::STATE_BIT::ATTACK2) && anim->IsAnimationTimeInRange(END_TIME_) ||
       anim->GetFlag(AnimationController::STATE_BIT::SMALL_HIT) || anim->GetFlag(AnimationController::STATE_BIT::BIG_HIT) ||
       !anim->GetFlag(AnimationController::STATE_BIT::ATTACK2))
    {
        // 削除
        GetOwner()->RemoveComponent(shared_from_this());

        // 移動を有効化
        if(auto ctrl = GetOwner()->GetComponent<Move>())
        {
            ctrl->SetEnable(true);
        }
    }

    // コンボ攻撃の判定時間まで
    if(anim->IsAnimationTimeInRange(0.0f, COMBO_LIMIT_TIME_))
    {
        if(auto sta = GetOwner()->GetComponent<Stamina>())
        {
            if(sta->GetStamina() > 0)
            {
                // 軽攻撃に移行する
                if(PressedAttackButton())
                {
                    go_next_attack_ = true;
                }
            }
        }
    }

    // 次のコンボに行く処理
    if(anim->IsAnimationTimeInRange(COMBO_START_TIME_))
    {
        // 軽攻撃に移行する処理
        if(go_next_attack_)
        {
            // 削除
            GetOwner()->RemoveComponent(shared_from_this());
            // 攻撃コンポーネント追加
            GetOwner()->AddComponent<Attack3>();

            // スタミナの消耗処理
            if(auto sta = GetOwner()->GetComponent<Stamina>())
            {
                sta->SetStaminaCoolTime();
                sta->SubStamina(NEXT_ATTACK_STAMINA_);
            }
        }
    }

    // 攻撃判定を付ける処理
    SetHit(ATTACK_START_TIME_);
    // 攻撃判定を消す処理
    SetNoHit(ATTACK_LIMIT_TIME_);
}

//! @brief GUI表示処理
void Attack2::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("Attack2"))
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

//! @brief 終了
void Attack2::Exit()
{
    Super::Exit();

    // アニメーションフラグをリセット
    if(auto anim = GetOwner()->GetComponent<AnimationController>())
    {
        anim->ResetFlag(AnimationController::STATE_BIT::ATTACK2);
    }

    // 攻撃判定を消す
    if(auto weapon = Scene::Object::Get<Stick>())
    {
        if(auto wea_atk = weapon->GetComponent<WeaponAttack>())
        {
            wea_atk->SetNoHit();
        }
    }
    GetPlayerOwner()->SetActionEnable(true);
}

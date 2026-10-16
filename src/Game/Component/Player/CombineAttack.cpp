//===========================================================================
//! @file CombineAttack.cpp
//! @brief プレイヤーの武器組み立て攻撃
//===========================================================================
#include "CombineAttack.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Object/Weapon/Stick.h>
#include <Game/Component/Player/AnimationController.h>
#include <Game/Component/Player/Stamina.h>
#include <Game/Component/Player/Move.h>
#include <Game/Component/WeaponAttack.h>
#include <Game/Component/Sound.h>
#include <Game/Component/Weapon/AttachAttack.h>
#include <Game/Object/Player/Player.h>

//! @brief 初期化
void CombineAttack::Init()
{
    Super::Init();

    // 移動を無効化
    if(auto ctrl = GetOwner()->GetComponent<Move>())
    {
        ctrl->SetEnable(false);
    }

    // アニメーションフラグを立つ
    if(auto anim = GetOwner()->GetComponent<AnimationController>())
    {
        anim->SetFlag(AnimationController::STATE_BIT::ATTACK1);
    }

    // SEを再生
    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        int random_sound       = GetRand(1) + 1;
        std::string sound_name = "attack" + std::to_string(random_sound);
        se->Play(sound_name);
    }
}

//! @brief 更新
void CombineAttack::Update()
{
    Super::Update();

    // 移動を無効化
    if(auto ctrl = GetOwner()->GetComponent<Move>())
    {
        ctrl->SetEnable(false);
    }

    // コンポーネントを取得
    auto time = GetOwner()->GetComponent<ComponentModel>();
    auto anim = GetOwner()->GetComponent<AnimationController>();
    if(!anim)
    {
        return;
    }

    // 攻撃が終わた場合
    // 攻撃を受けた場合
    if(anim->GetFlag(AnimationController::STATE_BIT::ATTACK1) && time->GetAnimationTime() > END_TIME_ / time->GetAnimationSpeed() ||
       anim->GetFlag(AnimationController::STATE_BIT::SMALL_HIT) || anim->GetFlag(AnimationController::STATE_BIT::BIG_HIT))
    {
        GetOwner()->RemoveComponent(shared_from_this());

        if(auto ctrl = GetOwner()->GetComponent<Move>())
        {
            ctrl->SetEnable(true);
        }
    }

    // 攻撃判定を付ける処理
    SetAttach();
}

//! @brief GUI表示処理
void CombineAttack::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("CombineAttack"))
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
void CombineAttack::Exit()
{
    Super::Exit();
    if(auto anim = GetOwner()->GetComponent<AnimationController>())
    {
        anim->ResetFlag(AnimationController::STATE_BIT::ATTACK1);
    }
    if(auto weapon = Scene::Object::Get<Stick>())
    {
        // アタッチ攻撃判定を外す
        if(weapon->GetComponent<AttachAttack>())
        {
            weapon->RemoveComponent<AttachAttack>();
        }
    }
    if(auto player = std::dynamic_pointer_cast<Player>(GetOwnerPtr()))
    {
        // 行動可能にする
        player->SetActionEnable(true);
    }
}

void CombineAttack::SetAttach()
{
    // モデルコンポーネントを取得
    auto anim = GetOwner()->GetComponent<AnimationController>();
    if(!anim)
    {
        return;
    }

    // アニメーションの時間が開始時間を超えたら攻撃判定を付ける
    if(anim->IsAnimationTimeInRange(ATTACK_START_TIME_))
    {
        if(auto weapon = Scene::Object::Get<Stick>())
        {
            if(!weapon->GetComponent<AttachAttack>())
            {
                weapon->AddComponent<AttachAttack>();
            }
        }
    }
}

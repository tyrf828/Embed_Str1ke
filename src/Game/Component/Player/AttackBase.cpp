//===========================================================================
//! @file AttackBase.cpp
//! @brief プレイヤーの攻撃コンポーネントの基底クラス
//===========================================================================
#include "AttackBase.h"
#include <Game/Object/Weapon/Stick.h>
#include <Game/Component/WeaponAttack.h>
#include <Game/Component/Player/Stamina.h>
#include <System/Component/ComponentModel.h>
#include <Game/Component/Sound.h>
#include <Game/Component/Player/Move.h>
#include <Game/Object/Player/Player.h>
#include <Game/Object/Player/Camera.h>

//! @brief 初期化
//! @param anim_flag  設定するアニメションフラグ
//! @param atk        攻撃力
//! @param sound_name 音名
void AttackBase::Initialize(AnimationController::STATE_BIT anim_flag, int atk, std::string sound_name)
{
    // 移動を無効化
    if(auto ctrl = GetOwner()->GetComponent<Move>())
    {
        ctrl->SetEnable(false);
    }

    // アニメーションフラグを立つ
    if(auto anim = GetOwner()->GetComponent<AnimationController>())
    {
        anim->SetFlag(anim_flag);
    }

    // 攻撃力を設定
    if(auto weapon = Scene::Object::Get<Stick>())
    {
        if(auto wea_atk = weapon->GetComponent<WeaponAttack>())
        {
            wea_atk->SetBaseATK(atk);
        }
    }

    // SEを再生
    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        se->Play(sound_name);
    }
    GetPlayerOwner()->SetActionEnable(false);
}

void AttackBase::Update()
{
    Super::Update();
    if(auto cam = Scene::Object::Get<Camera>())
    {
        if(auto target = cam->GetLockTarget())
        {
            auto model = GetOwner()->GetComponent<ComponentModel>();
            // ロックしている敵の方向を向く
            float3 vec = target->GetTranslate() - GetOwner()->GetTranslate();
            vec.y      = 0.0f;
            model->SetRotationToVectorWithLimit(vec, 5.0f);
        }
    }
}

//! @brief GUI表示処理
void AttackBase::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("AttackBase"))
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

//! @brief 攻撃判定を付ける
//! @param start_time 開始時間
void AttackBase::SetHit(const float& start_time)
{
    // モデルコンポーネントを取得
    auto time = GetOwner()->GetComponent<ComponentModel>();
    if(!time)
    {
        return;
    }

    // アニメーションの時間が開始時間を超えたら攻撃判定を付ける
    if(time->GetAnimationTime() >= start_time / time->GetAnimationSpeed())
    {
        if(auto weapon = Scene::Object::Get<Stick>())
        {
            if(auto wea_atk = weapon->GetComponent<WeaponAttack>())
            {
                // 攻撃判定まだ付けていない
                if(!attack_tigger_)
                {
                    // 攻撃判定を有効化
                    wea_atk->SetHit();
                    attack_tigger_ = true;
                }
            }
        }
    }
}

//! @brief 攻撃判定を消す
//! @param limit_time 終了時間
void AttackBase::SetNoHit(const float& limit_time)
{
    // モデルコンポーネントを取得
    auto time = GetOwner()->GetComponent<ComponentModel>();
    if(!time)
    {
        return;
    }

    // アニメーションの時間が終了時間を超えたら攻撃判定を付ける
    if(time->GetAnimationTime() >= limit_time / time->GetAnimationSpeed())
    {
        if(auto weapon = Scene::Object::Get<Stick>())
        {
            if(auto wea_atk = weapon->GetComponent<WeaponAttack>())
            {
                // 攻撃判定を無効化
                wea_atk->SetNoHit();
            }
        }
    }
}

//! @brief プレイヤを取得
//! @return プレイヤのポインタ
PlayerPtr AttackBase::GetPlayerOwner()
{
    return std::dynamic_pointer_cast<Player>(GetOwnerPtr());
}

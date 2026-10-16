//===========================================================================
//! @file Regenerate.cpp
//! @brief プレイヤーの回復コンポーネント
//===========================================================================
#include "Regenerate.h"
#include <Game/Component/Player/AnimationController.h>
#include <Game/Component/Player/Move.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Object/Bottle.h>
#include <System/Component/ComponentModel.h>
#include <Game/Component/Sound.h>
#include <Game/Object/Player/Player.h>
#include <System/Utils/IniFileLib.h>

//! @brief 初期化
void Regenerate::Init()
{
    Super::Init();

    if(auto ctrl = GetOwner()->GetComponent<Move>())
    {
        // 移動を無効
        ctrl->SetEnable(false);
    }

    if(auto anim = GetOwner()->GetComponent<AnimationController>())
    {
        // アニメーションの回復フラグを立つ
        anim->SetFlag(AnimationController::STATE_BIT::REGEN);
    }

    // 回復のアイテムを生成
    Scene::Object::Create<Bottle>("Bottle");

    if(auto player = std::dynamic_pointer_cast<Player>(GetOwnerPtr()))
    {
        player->SetRegenerateHP(true);
        IniFileLib ini("Game/PlayerData/PlayerData.ini");
        ini.WriteInt("Player", "Bottle", player->GetHPBottleAmount());
    }
}

//! @brief 更新
void Regenerate::Update()
{
    Super::Update();

    // コンポーネントを取得
    auto anim  = GetOwner()->GetComponent<AnimationController>();
    auto model = GetOwner()->GetComponent<ComponentModel>();

    if(!anim || !model)
    {
        return;
    }

    if(auto ctrl = GetOwner()->GetComponent<Move>())
    {
        // 移動を無効化
        ctrl->SetEnable(false);
    }

    // 回復アニメーションが再生中の場合
    if(anim->GetNowPlaying((int)AnimationController::STATE::REGEN))
    {
        // 回復を始める時間を超えたら
        if(anim->IsAnimationTimeInRange(REGEN_TIME_))
        {
            if(!regen_trigger_)
            {
                if(auto hp = GetOwner()->GetComponent<ComponentHP>())
                {
                    // HP回復
                    hp->AddHP(REGEN_HP_);
                    IniFileLib ini("Game/PlayerData/PlayerData.ini");
                    ini.WriteInt("Player", "HP", hp->GetHP());

                    regen_trigger_ = true;
                }
                // 音を再生
                if(auto se = GetOwner()->GetComponent<Sound>())
                {
                    se->Play("regen");
                }
            }
        }
    }
    else
    {
        // 削除
        GetOwner()->RemoveComponent(shared_from_this());
    }
}

//! @brief GUI表示処理
void Regenerate::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("Regenerate"))
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
void Regenerate::Exit()
{
    Super::Exit();

    if(auto anim = GetOwner()->GetComponent<AnimationController>())
    {
        anim->ResetFlag(AnimationController::STATE_BIT::REGEN);
    }

    if(auto ctrl = GetOwner()->GetComponent<Move>())
    {
        // 移動可能
        ctrl->SetEnable(true);
    }
    if(auto bottle = Scene::Object::Get<Bottle>("Bottle"))
    {
        // 回復のアイテムを削除
        Scene::Object::Release(bottle);
    }
    if(auto player = std::dynamic_pointer_cast<Player>(GetOwnerPtr()))
    {
        player->SetActionEnable(true);
        player->SetRegenerateHP(false);
    }
}

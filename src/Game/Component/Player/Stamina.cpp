//===========================================================================
//! @file Stamina.cpp
//! @brief プレイヤーのスタミナ
//===========================================================================
#include "Stamina.h"

//! @brief 初期化
//! @return 初期化済み
void Stamina::Init()
{
    Super::Init();
    // 初期化
    stamina_ = GetMaxStamina();
}

//! @brief 更新
void Stamina::Update()
{
    Super::Update();
    // クールタイム中の場合
    if(stamina_cool_time_ > 0)
    {
        // クールタイムを減らす
        stamina_cool_time_ -= GetDeltaTime();
    }
    else
    {
        // クールタイムを0にする
        stamina_cool_time_ = 0;
        // スタミナを回復
        stamina_ += static_cast<int>(STAMINA_RECOVER_SPEED_ * GetDeltaTime60());
    }
    // 範囲制限
    stamina_ = std::clamp(stamina_, 0, GetMaxStamina());
}

//! @brief GUI表示処理
void Stamina::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("ComponentStamina"))
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

//! @brief スタミナを取得
//! @return スタミナ
int Stamina::GetStamina() const
{
    return stamina_;
}

//! @brief スタミナを減らす
//! @param stamina スタミナを減らす設定量
void Stamina::SubStamina(int stamina)
{
    stamina_ -= stamina;
}

//! @brief 最大スタミナを取得
//! @return 最大スタミナ
int Stamina::GetMaxStamina() const
{
    return MAX_STAMINA_;
}

//! @brief スタミナ回復のクールタイムを取得
//! @return 回復のクールタイム
float Stamina::GetStaminaCoolTime() const
{
    return stamina_cool_time_;
}

//! @brief スタミナ回復のクールタイムを最大数に設定
void Stamina::SetStaminaCoolTime()
{
    stamina_cool_time_ = STAMINA_COOL_TIME_MAX_;
}

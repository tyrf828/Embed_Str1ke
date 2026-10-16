//===========================================================================
//! @file UltimateGauge.cpp
//! @brief プレイヤーの必殺技エネルギーゲージコンポーネント
//===========================================================================
#include "UltimateGauge.h"
#include <Game/Component/Player/UltimateAttack/UltimateAttack.h>
#include <System/Utils/IniFileLib.h>

//! @brief 初期化
void UltimateGauge::Init()
{
    Super::Init();
}

//! @brief 更新
void UltimateGauge::Update()
{
    Super::Update();
}

//! @brief GUI表示処理
void UltimateGauge::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("UltimateGauge"))
        {
            ImGui::SliderInt("Energy", &ultimate_energy_, 0, ULTIMATE_COST_);
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
void UltimateGauge::Exit()
{
    Super::Exit();
}

//! @brief エネルギーを追加
//! @param energy 追加するエネルギー値
void UltimateGauge::AddEnergy(int energy)
{
    ultimate_energy_ += energy;
    ultimate_energy_  = std::clamp(ultimate_energy_, 0, ULTIMATE_COST_);
    // 値をファイルに保存
    IniFileLib ini("Game/PlayerData/PlayerData.ini");
    ini.WriteInt("Player", "Ultimate", ultimate_energy_);
}

//! @brief エネルギーを追加
//! @param energy 追加するエネルギー値
void UltimateGauge::SetEnergy(int energy)
{
    ultimate_energy_ = energy;
    ultimate_energy_ = std::clamp(ultimate_energy_, 0, ULTIMATE_COST_);
    // 値をファイルに保存
    IniFileLib ini("Game/PlayerData/PlayerData.ini");
    ini.WriteInt("Player", "Ultimate", ultimate_energy_);
}

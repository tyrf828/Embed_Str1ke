//===========================================================================
//! @file AttachWeaponRemain.cpp
//! @brief アタッチ武器使える回数
//===========================================================================
#include "AttachWeaponRemain.h"

//! @brief 初期化
void AttachWeaponRemain::Init()
{
    Super::Init();
}

//! @brief 更新
void AttachWeaponRemain::Update()
{
    Super::Update();
    // 範囲制限
    remain_ = std::clamp(remain_, 0, max_remain_);

    if(remain_ <= 0)
    {
        // 残り回数が0になったら削除
        GetOwner()->RemoveComponent(shared_from_this());
    }
}

//! @brief 更新後
void AttachWeaponRemain::PostUpdate()
{
    Super::PostUpdate();
    // 範囲制限
    remain_ = std::clamp(remain_, 0, max_remain_);
}

//! @brief GUI表示処理
void AttachWeaponRemain::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("AttachWeaponRemain"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::DragInt("Remain", &remain_, 1.0f, 0, 100, "%d");
            ImGui::DragInt("Max Remain", &max_remain_, 1.0f, 1, 100, "%d");
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

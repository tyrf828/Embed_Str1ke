//===========================================================================
//! @file ComponentHP.cpp
//! @brief HPのベースコンポーネント
//===========================================================================
#include "ComponentHP.h"

void ComponentHP::GUI()
{
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("ComponentHP"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::DragInt("HP", &hp_, 1.0f, 0, max_hp_);
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//! @brief HPを取得
//! @return HP
int ComponentHP::GetHP() const
{
    return hp_;
}

//! @brief HPを減らす
void ComponentHP::SubHP(int hp)
{
    hp_ -= hp;
    // 範囲制限
    hp_ = std::clamp(hp_, 0, max_hp_);
}

//! @brief HPを増やす
void ComponentHP::AddHP(int hp)
{
    hp_ += hp;
    // 範囲制限
    hp_ = std::clamp(hp_, 0, max_hp_);
}

ComponentHPPtr ComponentHP::SetHP(int hp)
{
    hp_ = hp;
    hp_ = std::clamp(hp_, 0, max_hp_);
    return std::dynamic_pointer_cast<ComponentHP>(shared_from_this());
}

//! @brief 最大HPを設定
//! @param max_hp 最大HP
ComponentHPPtr ComponentHP::SetMaxHP(int max_hp)
{
    max_hp_ = max_hp;
    return std::dynamic_pointer_cast<ComponentHP>(shared_from_this());
}

int ComponentHP::GetMaxHP() const
{
    return max_hp_;
}

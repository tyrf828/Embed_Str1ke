#pragma once
//===========================================================================
//! @file AttackParam.h
//! @brief 敵攻撃パラメータ
//===========================================================================

//---------------------------------------------------------------------------
//! @class AttackParam
//! @brief 敵攻撃パラメータ
//---------------------------------------------------------------------------
class AttackParam : public Component
{
public:
    BP_COMPONENT_DECL(AttackParam, u8"敵攻撃パラメータ");

    //! @brief 初期化
    void Init() override
    {
        Super::Init();
    }

    //! @brief GUI表示処理
    void GUI() override
    {
        Super::GUI();
        ImGui::Begin(GetOwner()->GetName().data());
        {
            ImGui::Separator();
            if(ImGui::TreeNode("AttackParam"))
            {
                if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
                {
                    GetOwner()->RemoveComponent(shared_from_this());
                }
                ImGui::InputInt("Attack", &attack_);
                ImGui::TreePop();
            }
        }
        ImGui::End();
    }

    //! @brief 攻撃力を設定
    //! @param atk 攻撃力
    void SetAttack(int atk)
    {
        attack_ = atk;
    }

    //! @brief 攻撃力を取得
    //! @return 攻撃力
    int GetAttack() const
    {
        return attack_;
    }

private:
    int attack_ = 0;   //!< 攻撃力
};

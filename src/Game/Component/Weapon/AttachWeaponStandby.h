#pragma once
//===========================================================================
//! @file AttachWeaponStandby.h
//! @brief 武器待機
//===========================================================================

USING_PTR(AttachWeaponStandby);

//---------------------------------------------------------------------------
//! @class AttachWeaponStandby
//! @brief 武器待機
//---------------------------------------------------------------------------
class AttachWeaponStandby : public Component
{
public:
    BP_COMPONENT_DECL(AttachWeaponStandby, u8"武器待機");
    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;
};

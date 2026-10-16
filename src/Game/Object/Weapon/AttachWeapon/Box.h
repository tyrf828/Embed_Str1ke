//===========================================================================
//! @file Box.h
//! @brief プレイヤーの武器
//===========================================================================
#pragma once
#include <Game/Object/Weapon/AttachWeapon/AttachWeaponBase.h>

//---------------------------------------------------------------------------
//! @class Box
//! @brief 箱
//---------------------------------------------------------------------------
class Box : public AttachWeaponBase
{
public:
    BP_OBJECT_DECL(Box, "Box");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

private:
};

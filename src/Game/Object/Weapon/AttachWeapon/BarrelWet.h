//===========================================================================
//! @file BarrelWet.h
//! @brief プレイヤーの武器
//===========================================================================
#pragma once
#include <Game/Object/Weapon/AttachWeapon/AttachWeaponBase.h>

//---------------------------------------------------------------------------
//! @class BarrelWet
//! @brief 木造バレル
//---------------------------------------------------------------------------
class BarrelWet : public AttachWeaponBase
{
public:
    BP_OBJECT_DECL(BarrelWet, "BarrelWet");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

private:
};

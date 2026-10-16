//===========================================================================
//! @file Barrel.h
//! @brief プレイヤーの武器
//===========================================================================
#pragma once
#include <Game/Object/Weapon/AttachWeapon/AttachWeaponBase.h>

//---------------------------------------------------------------------------
//! @class Barrel
//! @brief バレル
//---------------------------------------------------------------------------
class Barrel : public AttachWeaponBase
{
public:
    BP_OBJECT_DECL(Barrel, "Barrel");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

private:
};

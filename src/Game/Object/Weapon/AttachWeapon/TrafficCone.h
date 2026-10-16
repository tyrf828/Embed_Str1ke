//===========================================================================
//! @file TrafficCone.h
//! @brief プレイヤーの武器
//===========================================================================
#pragma once
#include <Game/Object/Weapon/AttachWeapon/AttachWeaponBase.h>

//---------------------------------------------------------------------------
//! @class TrafficCone
//! @brief コーン
//---------------------------------------------------------------------------
class TrafficCone : public AttachWeaponBase
{
public:
    BP_OBJECT_DECL(TrafficCone, "TrafficCone");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

private:
};

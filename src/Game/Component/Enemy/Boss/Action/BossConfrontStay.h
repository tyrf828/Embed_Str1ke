#pragma once
//===========================================================================
//! @file BossConfrontStay.h
//! @brief ボスの停止コンポーネント
//===========================================================================
#include "BossConfront.h"

//---------------------------------------------------------------------------
//! @class BossConfrontStay
//! @brief ボスの停止コンポーネント
//---------------------------------------------------------------------------
class BossConfrontStay : public BossConfrontFactory
{
public:
    BP_COMPONENT_DECL(BossConfrontStay, u8"ボスの停止コンポーネント");

    //! @brief 初期化
    void ConfrontInit() override;

    //! @brief 更新
    void ConfrontUpdate() override;

    //! @brief 終了
    void ConfrontExit() override;
};

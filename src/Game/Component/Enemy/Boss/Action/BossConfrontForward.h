#pragma once
//===========================================================================
//! @file BossConfrontForward.h
//! @brief ボスの前進コンポーネント
//===========================================================================
#include "BossConfront.h"

//---------------------------------------------------------------------------
//! @class BossConfrontForward
//! @brief ボスの前進コンポーネント
//---------------------------------------------------------------------------
class BossConfrontForward : public BossConfrontFactory
{
public:
    BP_COMPONENT_DECL(BossConfrontForward, u8"ボスの前進コンポーネント");

    //! @brief 初期化
    void ConfrontInit() override;

    //! @brief 更新
    void ConfrontUpdate() override;

    //! @brief 終了
    void ConfrontExit() override;
};

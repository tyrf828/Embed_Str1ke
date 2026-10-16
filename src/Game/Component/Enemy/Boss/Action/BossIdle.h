#pragma once
//===========================================================================
//! @file BossIdle.h
//! @brief ボスの待機コンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class BossIdle
//! @brief ボスの待機コンポーネント
//---------------------------------------------------------------------------
class BossIdle : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(BossIdle, u8"ボスの待機コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

private:
};

#pragma once
//===========================================================================
//! @file CloseEnemyIdle.h
//! @brief 敵の待機コンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class CloseEnemyIdle
//! @brief 敵の待機コンポーネント
//---------------------------------------------------------------------------
class CloseEnemyIdle : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(CloseEnemyIdle, u8"敵の待機コンポーネント");

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

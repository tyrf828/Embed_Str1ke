#pragma once
//===========================================================================
//! @file FarEnemyStayby.h
//! @brief 敵の待機準備コンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class FarEnemyStayby
//! @brief 敵の待機準備コンポーネント
//---------------------------------------------------------------------------
class FarEnemyStayby : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(FarEnemyStayby, u8"敵の待機準備コンポーネント");

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

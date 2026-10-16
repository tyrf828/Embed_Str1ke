#pragma once
//===========================================================================
//! @file CloseEnemyDeath.h
//! @brief 敵死亡コンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class CloseEnemyDeath
//! @brief 敵死亡コンポーネント
//---------------------------------------------------------------------------
class CloseEnemyDeath : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(CloseEnemyDeath, u8"敵死亡コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 遷移可能か
    //! @return 常にfalse
    bool CanTransit() const override
    {
        return false;
    }

private:
};

#pragma once
//===========================================================================
//! @file CloseEnemyKnockback.h
//! @brief 敵のノックバックコンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class CloseEnemyKnockback
//! @brief 敵のノックバックアクション
//---------------------------------------------------------------------------
class CloseEnemyKnockback : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(CloseEnemyKnockback, u8"敵のノックバックコンポーネント");

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

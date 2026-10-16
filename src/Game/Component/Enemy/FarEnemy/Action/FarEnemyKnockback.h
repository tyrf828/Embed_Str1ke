#pragma once
//===========================================================================
//! @file FarEnemyKnockback.h
//! @brief 敵のノックバックコンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class FarEnemyKnockback
//! @brief 敵のノックバックアクション
//---------------------------------------------------------------------------
class FarEnemyKnockback : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(FarEnemyKnockback, u8"敵のノックバックコンポーネント");

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

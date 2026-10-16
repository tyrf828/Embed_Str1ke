#pragma once
//===========================================================================
//! @file FarEnemyDamaged.h
//! @brief 敵がダメージ受けたコンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class FarEnemyDamaged
//! @brief 敵がダメージ受けたコンポーネント
//---------------------------------------------------------------------------
class FarEnemyDamaged : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(FarEnemyDamaged, u8"敵がダメージ受けたコンポーネント");

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

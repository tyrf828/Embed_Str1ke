#pragma once
//===========================================================================
//! @file CloseEnemyStandUp.h
//! @brief 敵の立ち上がりコンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class CloseEnemyStandUp
//! @brief 敵の立ち上がりアクション
//---------------------------------------------------------------------------
class CloseEnemyStandUp : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(CloseEnemyStandUp, u8"敵の立ち上がりコンポーネント");

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

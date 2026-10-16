#pragma once
//===========================================================================
//! @file BossDamaged.h
//! @brief ボスがダメージ受けたコンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class BossDamaged
//! @brief ボスがダメージ受けたコンポーネント
//---------------------------------------------------------------------------
class BossDamaged : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(BossDamaged, u8"ボスがダメージ受けたコンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief このアクションに遷移可能か
    //! @return 常にfalse
    bool CanTransit() const override
    {
        return false;
    }

private:
};

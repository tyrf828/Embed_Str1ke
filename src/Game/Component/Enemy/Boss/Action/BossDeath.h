#pragma once
//===========================================================================
//! @file BossDeath.h
//! @brief ボス死亡コンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class BossDeath
//! @brief ボス死亡コンポーネント
//---------------------------------------------------------------------------
class BossDeath : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(BossDeath, u8"ボス死亡コンポーネント");

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
    static constexpr float DESTROY_TIME_MAX_ = 1.5f;   //!< 破壊するまでの最大時間
    float destroy_count_                     = 0.0f;   //!< 破壊するまでのカウント
};

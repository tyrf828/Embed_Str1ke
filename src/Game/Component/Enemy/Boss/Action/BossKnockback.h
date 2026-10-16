#pragma once
//===========================================================================
//! @file BossKnockback.h
//! @brief ボスのノックバックコンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class BossKnockback
//! @brief ボスのノックバックコンポーネント
//---------------------------------------------------------------------------
class BossKnockback : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(BossKnockback, u8"ボスのノックバックコンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

private:
    bool is_in_knockback_ = false;   //!< ノックバック中か(補間対策)
};

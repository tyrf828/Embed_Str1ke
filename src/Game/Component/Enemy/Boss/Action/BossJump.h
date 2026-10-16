#pragma once
//===========================================================================
//! @file BossJump.h
//! @brief ボスのジャンプ動作コンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class BossJump
//! @brief ボスのジャンプ動作コンポーネント
//---------------------------------------------------------------------------
class BossJump : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(BossJump, u8"ボスのジャンプ動作コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief ジャンプ力を設定
    //! @param force ジャンプ力
    void SetJumpForce(float force)
    {
        jump_force_ = force;
    }

private:
    float jump_force_   = 5.0f;   //!< ジャンプ力
    float elapsed_time_ = 0.0f;   //!< 経過時間
};

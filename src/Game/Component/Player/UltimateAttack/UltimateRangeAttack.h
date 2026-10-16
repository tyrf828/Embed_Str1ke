#pragma once
//===========================================================================
//! @file UltimateRangeAttack.h
//! @brief 必殺技範囲攻撃
//===========================================================================
#include <Game/Component/Player/UltimateAttack/UltimateBase.h>

//---------------------------------------------------------------------------
//! @class UltimateRangeAttack
//! @brief 必殺技範囲攻撃
//---------------------------------------------------------------------------
class UltimateRangeAttack : public UltimateBase
{
public:
    BP_COMPONENT_DECL(UltimateRangeAttack, u8"必殺技範囲攻撃");
    //! @brief 初期化
    void Init() override;
    //! @brief 更新
    void Update() override;
    //! @brief 描画
    void LateDraw() override;
    //! @brief 終了
    void Exit() override;

private:
    float effect_spawn_elapsed_time_   = 0.0f;    //!< エフェクト生成の経過時間
    static constexpr int EFFECT_MAX_   = 10;      //!< エフェクトの最大数
    int effect_num_                    = 0;       //!< エフェクトの生成数
    static constexpr float SPAWN_TIME_ = 0.15f;   //!< 生成間隔
    float next_state_time              = 0.0f;    //!< 次の状態への遷移時間
};

#pragma once
//===========================================================================
//! @file UltimateDash.h
//! @brief 必殺技ダッシュ
//===========================================================================
#include <Game/Component/Player/UltimateAttack/UltimateBase.h>

//---------------------------------------------------------------------------
//! @class UltimateDash
//! @brief 必殺技ダッシュ
//---------------------------------------------------------------------------
class UltimateDash : public UltimateBase
{
public:
    BP_COMPONENT_DECL(UltimateDash, u8"必殺技ダッシュ");
    //! @brief 初期化
    void Init() override;
    //! @brief 更新
    void Update() override;
    //! @brief 描画
    void LateDraw() override;
    //! @brief 終了
    void Exit() override;

private:
    static constexpr float DASH_TIME_ = 0.3f;   //!< ダッシュ時間
    float dash_time_                  = 0.0f;   //!< ダッシュ経過時間
    float alpha_                      = 0.0f;   //!< 透明度
};

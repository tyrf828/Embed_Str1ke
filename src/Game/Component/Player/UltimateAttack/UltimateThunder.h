#pragma once
//===========================================================================
//! @file UltimateThunder.h
//! @brief 必殺技の雷攻撃
//===========================================================================
#include <Game/Component/Player/UltimateAttack/UltimateBase.h>

//---------------------------------------------------------------------------
//! @class UltimateThunder
//! @brief 必殺技の雷攻撃
//---------------------------------------------------------------------------
class UltimateThunder : public UltimateBase
{
public:
    BP_COMPONENT_DECL(UltimateThunder, u8"必殺技の雷攻撃");
    //! @brief 初期化
    void Init() override;
    //! @brief 更新
    void Update() override;
    //! @brief 描画
    void LateDraw() override;
    //! @brief 終了
    void Exit() override;

private:
    int thunder_alpha_ = 255;    //!< 雷の透明度
    float alpha_       = 0.0f;   //!< 透明度
    float slash_time_  = 0.0f;   //!< 斬撃時間
    float slash_zoom_  = 1.0f;   //!< 斬撃ズーム
};

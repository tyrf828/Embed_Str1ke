//===========================================================================
//! @file MentalPosBall.h
//! @brief メンタルマップ用の当たり判定
//===========================================================================
#pragma once

//---------------------------------------------------------------------------
//! @class MentalPosBall
//! @brief メンタルマップ用の当たり判定
//---------------------------------------------------------------------------
class MentalPosBall : public Object
{
public:
    BP_OBJECT_DECL(MentalPosBall, u8"メンタルマップ用の当たり判定");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 遅延描画
    void LateDraw() override;

private:
};

#pragma once
//===========================================================================
//! @file UltimateAttackEffect.h
//! @brief 必殺技エフェクト
//===========================================================================
#include "EffectBase.h"
USING_PTR(UltimateAttackEffect);

//---------------------------------------------------------------------------
//! @class UltimateAttackEffect
//! @brief 必殺技エフェクト
//---------------------------------------------------------------------------
class UltimateAttackEffect : public EffectBase
{
public:
    BP_OBJECT_DECL(UltimateAttackEffect, "UltimateAttackEffect");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 攻撃方向の設定
    void SetVectorAttackDir(float3 dir = {0, 1, 0})
    {
        attack_dir_ = dir;
        SetRotationToVector(attack_dir_, {0, 1, 0}, true);
    }

private:
    float init_height_             = 0.0f;                 //!< 初期位置の高さ
    static constexpr float HEIGHT_ = 30.0f;                //!< 移動する高さ
    float3 attack_dir_             = {0.0f, 0.0f, 0.0f};   //!< 攻撃方向
    static constexpr float SPEED_  = 2.5f;                 //!< 移動速度
};

#pragma once
//===========================================================================
//! @file ReactiveForce.h
//! @brief 武器反動力
//===========================================================================

#include <Game/Component/ForceMoveBase.h>

//---------------------------------------------------------------------------
//! @class ReactiveForce
//! @brief 武器反動力
//---------------------------------------------------------------------------
class ReactiveForce : public ForceMoveBase
{
public:
    BP_COMPONENT_DECL(ReactiveForce, u8"武器反動力");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 描画
    void LateDraw() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 終了
    void Exit() override;

    //! @brief 方向を設定
    //! @param dir 方向
    //! @param force_power 力の強さ
    //! @param rot_power 回転の強さ
    void SetDirection(float3 dir, float force_power = 2.0f, float rot_power = 20.0f)
    {
        dir_         = normalize(dir);
        force_power_ = force_power;
        rot_power_   = rot_power;
    }

private:
    float3 force_axis_                    = {0.0f, 0.0f, 0.0f};   //!< 力の軸
    float rot_power_                      = 5.0f;                 //!< 回転の強さ
    float life_time_                      = 0.0f;                 //!< 生存時間
    static constexpr float LIFE_TIME_MAX_ = 5.0f;                 //!< 生存時間
};

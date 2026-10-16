//---------------------------------------------------------------------------
//! @file   LightPoint.h
//! @brief  点光源
//---------------------------------------------------------------------------
#pragma once

//===========================================================================
//! @class LightPoint
//! @brief 点光源
//===========================================================================
class LightPoint : public Object
{
public:
    BP_OBJECT_DECL(LightPoint, u8"点光源");

    //! @brief 初期化
    //! @return 初期化済み
    virtual bool Init() override;

    //! @brief 終了
    virtual void Exit() override;

    //! @brief GUI
    void GUI() override;

    //! @brief パラメーター設定
    //! @param color カラー
    //! @param radius 半径
    //! @param intensity 強度
    void Set(float4 color, float radius, float intensity = 30.0f)
    {
        info_.light_color_     = color;
        info_.light_radius_    = radius;
        info_.light_intensity_ = intensity;
    }

private:
    struct PointLightInfo
    {
        float3 light_position_ = float3(0.0f, 0.0f, 0.0f);         //!< 光源の位置座標
        float4 light_color_    = float4(1.0f, 1.0f, 1.0f, 0.0f);   //!< 光源のカラー
        float light_radius_    = 50.0f;                            //!< 光源の影響範囲半径
        float light_intensity_ = 30.0f;                            //!< 光源の強度
    };

    std::shared_ptr<ShaderPs> shader_ps_;   //!< ピクセルシェーダー
    int buffer_cb_ = -1;                    //!< 定数バッファ
    PointLightInfo info_;                   //!< 点光源情報
};

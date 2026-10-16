//---------------------------------------------------------------------------
//! @file   FilterVolumetricFog.h
//! @brief  ボリューメトリックフォグフィルター
//---------------------------------------------------------------------------
#pragma once

//===========================================================================
//! @class FilterVolumetricFog
//! @brief ボリューメトリックフォグ
//===========================================================================
class FilterVolumetricFog : public Object
{
public:
    BP_OBJECT_DECL(FilterVolumetricFog, u8"Volumetric Fog");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI描画
    void GUI() override;

    //! @brief フォグの有効か設定
    //! @param enable 有効か
    void SetEnable(bool enable)
    {
        is_enable_ = enable;
    }

    //! @brief 高さ設定
    //! @param base_height 高さ
    void SetHeight(float base_height = -500.0f)
    {
        info_.base_height_ = base_height;
    }

private:
    struct FogInfo
    {
        float density_        = 0.003f;    //!< 霧の密度
        float base_height_    = -500.0f;   //!< ベースの高さ
        float height_falloff_ = 0.015f;    //!< 高さの減衰率
        float anisotropy_     = 0.3f;      //!< 散乱方向(0~1)

        float light_intensity_    = 5.0f;                  //!< 光の強さ
        float light_direction_[3] = {1.0f, 1.0f, -1.0f};   //!< 光源へ向かう方向
    };

    std::shared_ptr<Texture> texture_source_;                         //!< HDRテクスチャ
    std::shared_ptr<ShaderPs> shader_ps_;                             //!< ピクセルシェーダー
    Microsoft::WRL::ComPtr<ID3D11SamplerState> d3d_shadow_sampler_;   //!< シャドウ用比較サンプラー

    bool is_enable_ = true;   //!< 有効フラグ
    FogInfo info_;            //!< フィルター情報
    int buffer_cb_ = -1;      //!< 定数バッファ
};

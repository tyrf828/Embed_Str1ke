//---------------------------------------------------------------------------
//! @file   FilterLensFlare.h
//! @brief  レンズフレアフィルター
//---------------------------------------------------------------------------
#pragma once

//===========================================================================
//! @class FilterLensFlare
//! @brief レンズフレアフィルター
//===========================================================================
class FilterLensFlare : public Object
{
public:
    BP_OBJECT_DECL(FilterLensFlare, u8"LensFlare");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 終了
    void Exit() override;

private:
    static constexpr int REDUCTION_MAX_ = 2;   //!< 縮小回数

    std::shared_ptr<Texture> texture_lens_color;   //!< レンズカラー

    std::shared_ptr<Texture> texture_work_;                                       //!< ワークテクスチャ
    std::array<std::shared_ptr<Texture>, 2> texture_reduction_[REDUCTION_MAX_];   //!< ゴーストテクスチャ

    std::shared_ptr<ShaderPs> shader_ps_nd_;                     //!< 減光フィルター
    std::shared_ptr<ShaderPs> shader_ps_gaussian_;               //!< ガウシアンブラー
    std::shared_ptr<ShaderPs> shader_ps_;                        //!< ブラー
    std::shared_ptr<ShaderPs> shader_ps_chromatic_aberration_;   //!< 色収差

    struct FilterInfo
    {
        float2 pixel_offset_;   //!< ピクセルオフセット
    };

    int buffer_cb_ = -1;   //!< 定数バッファ
};

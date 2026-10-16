//---------------------------------------------------------------------------
//! @file   FilterInkWash.h
//! @brief  水墨フィルター
//---------------------------------------------------------------------------
#pragma once

//===========================================================================
//! @class FilterInkWash
//! @brief 水墨フィルター
//===========================================================================
class FilterInkWash : public Object
{
public:
    BP_OBJECT_DECL(FilterInkWash, "Ink Wash");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 終了
    void Exit() override;

    //! @GUI
    void GUI() override;

private:
    struct InkWashInfo
    {
        float2 uv_pixel_;            //!< 1ピクセル分のUV幅
        float edge_width_  = 3.0f;   //!< 輪郭の幅
        float sensitivity_ = 0.4f;   //!< 法線のしきい値
        float noise_scale_ = 0.2f;   //!< ノイズの細かさ
        float padding_;
    };

    struct BlurInfo
    {
        float2 pixel_offset_;   //!< ピクセルオフセット
    };

    std::array<std::shared_ptr<Texture>, 2> texture_work_;   //!< ワークテクスチャ
    std::shared_ptr<ShaderPs> shader_ps_color_;              //< グレースケール
    std::shared_ptr<ShaderPs> shader_ps_gaussian_;           //< ガウシアンブラー
    std::shared_ptr<ShaderPs> shader_ps_;                    //!< ピクセルシェーダー
    std::shared_ptr<Texture> texture_noise_;                 //!< ノイズ

    InkWashInfo info_;       //!< フィルター情報
    int buffer_cb_   = -1;   //!< 定数バッファ
    int gaussian_cb_ = -1;   //!< 定数バッファ
};

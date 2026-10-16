//---------------------------------------------------------------------------
//! @file   FilterBloom.h
//! @brief  Bloomクレア
//---------------------------------------------------------------------------
#pragma once

//===========================================================================
//! @class FilterBloom
//! @brief Bloomクレアフィルター
//===========================================================================
class FilterBloom : public Object
{
public:
    BP_OBJECT_DECL(FilterBloom, u8"Bloom");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 終了
    void Exit() override;

private:
    std::shared_ptr<Texture> texture_work_;                                       //!< ワークテクスチャ
    static constexpr int REDUCTION_MAX_ = 5;                                      //!< 縮小回数
    std::array<std::shared_ptr<Texture>, 2> texture_reduction_[REDUCTION_MAX_];   //!< 縮小テクスチャ
    std::shared_ptr<ShaderPs> shader_ps_nd_;                                      //!< 減光フィルター
    std::shared_ptr<ShaderPs> shader_ps_gaussian_;                                //!< ガウシアンブラー
    std::shared_ptr<ShaderPs> shader_ps_;                                         //!< ブルーム合成

    struct FilterInfo
    {
        float2 pixel_offset_;   //!< ピクセルオフセット
    };
    FilterInfo info_;      //!< フィルター情報
    int buffer_cb_ = -1;   //!< 定数バッファ
};

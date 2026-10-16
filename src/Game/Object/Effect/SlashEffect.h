//===========================================================================
//! @file SlashEffect.h
//! @brief 画面分割分割エフェクト
//===========================================================================
#pragma once

USING_PTR(SlashEffect);

//---------------------------------------------------------------------------
//! @class SlashEffect
//! @brief 画面分割分割エフェクト
//---------------------------------------------------------------------------
class SlashEffect : public Object
{
public:
    BP_OBJECT_DECL(SlashEffect, u8"画面分割分割シェーダー");

    //! @brief 初期化
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 描画
    void Exit() override;

    //! @brief 分割率の設定
    void SetZoom(float z)
    {
        zoom_ = z;
    }

    //! @brief 分割率の取得
    //! @return 分割率
    float GetZoom() const
    {
        return zoom_;
    }

private:
    struct Info
    {
        float zoom_;     //!< 分割率
        float pad_[3];   //!< パディング
    };

    std::shared_ptr<Texture> texture_;      //!< 描画用テクスチャ
    std::shared_ptr<ShaderPs> shader_ps_;   //!< ピクセルシェーダー
    int info_cb_ = -1;                      //!< 定数バッファ

    static constexpr float ZOOM_MAX_ = 1.5f;   //!< 最大拡大率
    float zoom_                      = 1.0f;   //!< 拡大率
};

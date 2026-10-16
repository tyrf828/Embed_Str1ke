//---------------------------------------------------------------------------
//! @file   Shadowmap.h
//! @brief  シャドウマップ管理
//---------------------------------------------------------------------------
#pragma once

//===========================================================================
//! @class Shadowmap
//! @brief シャドウマップの管理
//===========================================================================
class Shadowmap : public Object
{
public:
    BP_OBJECT_DECL(Shadowmap, u8"シャドウマップ");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void PostUpdate() override;

    //! @brief 描画
    void Draw() override;

    //! @brief 終了
    void Exit() override;

    //! @brief シャドウマップの行列を取得
    //! @param index インデックス
    //! @return ビュー投影行列
    matrix GetShadowmapViewProj(int index) const
    {
        return shadowmap_view_proj_[index];
    }

    //! @brief シャドウマップテクスチャを取得
    //! @return テクスチャ
    Texture* GetShadowmapTexture() const
    {
        return shadowmap_color_atlas_.get();
    }

    //! @brief シャドウマップの高さを設定
    //! @param height 高さ
    void SetHeight(float height)
    {
        shadow_height_ = height;
    }

    //! @brief シャドウマップの方向を設定
    //! @param direction 方向
    void SetDirection(float3 direction = float3(1.0f, 1.0f, -1.0f))
    {
        shadow_direction_ = normalize(direction);
    }

    float3 GetDirection() const
    {
        return shadow_direction_;
    }

    static inline constexpr int CASCADE_NUM = 4;   //!< カスケード数

private:
    std::shared_ptr<Texture> shadowmap_color_;   //!< シャドウマップのテクスチャ
    std::shared_ptr<Texture> shadowmap_depth_;   //!< シャドウマップの深度バッファ

    std::shared_ptr<Texture> shadowmap_color_atlas_;   //!< シャドウマップのアトラス

    matrix shadowmap_view_ = matrix::identity();   //!< シャドウマップのビュー行列
    matrix shadowmap_proj_ = matrix::identity();   //!< シャドウマップの投影行列
    matrix shadowmap_view_proj_[CASCADE_NUM];      //!< シャドウマップのビュー投影行列

    ID3D11SamplerState* d3d_shadow_sampler_ = nullptr;   //!< シャドウサンプラー

    float shadow_height_     = 600.0f;                      //!< シャドウマップの高さ
    float3 shadow_direction_ = float3(1.0f, 1.0f, -1.0f);   //!< シャドウマップの方向
};

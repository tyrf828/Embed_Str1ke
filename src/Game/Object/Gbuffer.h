//---------------------------------------------------------------------------
//! @file   Gbuffer.h
//! @brief  GBuffer管理
//---------------------------------------------------------------------------
#pragma once

//===========================================================================
//! GBuffer管理
//===========================================================================
class GBuffer : public Object
{
public:
    BP_OBJECT_DECL(GBuffer, u8"GBuffer管理");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI描画
    void GUI() override;

    //! @brief GBufferのテクスチャを取得します
    //! @param index インデックス
    //! @return テクスチャ
    Texture* GetGBuffer(int index) const;

    //! @brief GBufferのテクスチャを設定します
    void SetGBufferTextures();

private:
    // GBuffer
    //            R         G         B        A
    // RT0              Albedo.rgb             AO      DXGI_FORMAT_R8G8B8A8_UNORM
    // RT1         Normal.xy      Roughness  Metallic  DXGI_FORMAT_R8G8B8A8_UNORM
    // RT2         Velocity.x           Velocity.y     DXGI_FORMAT_R16G16_FLOAT
    // RT3          WorldPosition.xyz        Decal     DXI_FORMAT_R32G32B32A32_FLOAT
    // Depth                   Depth                   DXGI_FORMAT_D32_FLOAT

    static constexpr int GBUFFER_NUM = 4;   //!< GBufferの数

    std::array<std::shared_ptr<Texture>, GBUFFER_NUM> gbuffer_texture_;   //!< GBufferテクスチャ

    //! コピーして使うデプスバッファのカラーバッファ
    std::shared_ptr<Texture> depth_texture_;
};

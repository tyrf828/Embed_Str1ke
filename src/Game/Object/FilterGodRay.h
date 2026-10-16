//---------------------------------------------------------------------------
//! @file   FilterGodRay.h
//! @brief  ゴッドレイフィルター
//---------------------------------------------------------------------------
#pragma once

//===========================================================================
//! @class FilterGodRay
//! @brief ゴッドレイ
//===========================================================================
class FilterGodRay : public Object
{
public:
    BP_OBJECT_DECL(FilterGodRay, u8"GodRay");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 終了
    void Exit() override;

private:
    struct GodRayInfo
    {
        float2 light_pos_;   //!< 画面上の光源位置
    };

    std::shared_ptr<Texture> texture_work_;   //!< ワークテクスチャ
    std::shared_ptr<ShaderPs> shader_ps_;     //!< ピクセルシェーダー

    GodRayInfo info_;
    int buffer_cb_ = -1;   //!< 定数バッファ
};

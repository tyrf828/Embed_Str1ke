//---------------------------------------------------------------------------
//! @file   LightEnvironment.h
//! @brief  環境光
//---------------------------------------------------------------------------
#pragma once

//===========================================================================
//! @class LightEnvironment
//! @brief 環境光
//===========================================================================
class LightEnvironment : public Object
{
public:
    BP_OBJECT_DECL(LightEnvironment, u8"環境光");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 終了
    void Exit() override;

private:
    std::shared_ptr<ShaderPs> shader_ps_;   //!< ピクセルシェーダー

    std::shared_ptr<Texture> texture_ibl_diffuse_;    //!< IBL Diffuseテクスチャ
    std::shared_ptr<Texture> texture_ibl_specular_;   //!< IBL Specularテクスチャ
    std::shared_ptr<Texture> texture_ibl_brdf_;       //!< IBL BRDFテクスチャ
};

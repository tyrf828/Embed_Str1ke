//---------------------------------------------------------------------------
//! @file   LightManager.h
//! @brief  Light管理
//---------------------------------------------------------------------------
#pragma once

//===========================================================================
//! @class LightManager
//! @brief Light管理
//===========================================================================
class LightManager : public Object
{
public:
    BP_OBJECT_DECL(LightManager, u8"Light管理");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI
    void GUI() override;

private:
    //! Light Accumulation テクスチャ [0]=Diffuse [1]=Specular
    std::array<std::shared_ptr<Texture>, 2> light_texture_;

    std::shared_ptr<ShaderPs> shader_ps_;   //!< ピクセルシェーダー
};

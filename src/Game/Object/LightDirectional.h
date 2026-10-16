//---------------------------------------------------------------------------
//! @file   LightDirectional.h
//! @brief  平行光源
//---------------------------------------------------------------------------
#pragma once

//===========================================================================
//! @class LightDirectional
//! @brief 平行光源
//===========================================================================
class LightDirectional : public Object
{
public:
    BP_OBJECT_DECL(LightDirectional, u8"平行光源");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 終了
    void Exit() override;

private:
    std::shared_ptr<ShaderPs> shader_ps_;   //!< ピクセルシェーダー
};

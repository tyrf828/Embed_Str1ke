//---------------------------------------------------------------------------
//! @file   SSAO.h
//! @brief  Screen Space Amibient Occlusion
//---------------------------------------------------------------------------
#pragma once

//===========================================================================
//! @class SSAO
//! @brief Screen Space Amibient Occlusion
//===========================================================================
class SSAO : public Object
{
public:
    BP_OBJECT_DECL(SSAO, u8"SSAO");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 終了
    void Exit() override;

private:
    std::shared_ptr<ShaderPs> shader_ps_;   //!< ピクセルシェーダー
};

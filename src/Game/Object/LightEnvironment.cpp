//---------------------------------------------------------------------------
//! @file   LightEnvironment.cpp
//! @brief  環境光
//---------------------------------------------------------------------------
#include "LightEnvironment.h"
#include "System/Graphics/Render.h"

//! @brief 初期化
//! @return 初期化済み
bool LightEnvironment::Init()
{
    SetName("LightEnvironment");

    // IBL読み込み
    texture_ibl_diffuse_  = std::make_shared<Texture>("data/IBL/iblDiffuseHDR.dds");
    texture_ibl_specular_ = std::make_shared<Texture>("data/IBL/iblSpecularHDR.dds");
    texture_ibl_brdf_     = std::make_shared<Texture>("data/IBL/iblBrdf.dds");

    // シェーダー初期化
    shader_ps_ = std::make_shared<ShaderPs>("data/Shader/ps_light_ibl.fx");

    {
        auto execute = [this]() {
            // IBLテクスチャを設定
            SetTexture(27, texture_ibl_diffuse_.get());
            SetTexture(28, texture_ibl_specular_.get());
            SetTexture(29, texture_ibl_brdf_.get());

            SetDrawBlendMode(DX_BLENDMODE_ADD, 255);

            // 全画面にシェーダーを適用
            FillRenderTarget(*shader_ps_);

            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, -1);
        };

        SetProc("LightEnvironment", execute, ProcTiming::Light, static_cast<ProcPriority>(32768));
    }

    return Super::Init();
}

//! @brief 終了
void LightEnvironment::Exit()
{
    Super::Exit();
    shader_ps_.reset();
    texture_ibl_diffuse_.reset();
    texture_ibl_specular_.reset();
    texture_ibl_brdf_.reset();
}

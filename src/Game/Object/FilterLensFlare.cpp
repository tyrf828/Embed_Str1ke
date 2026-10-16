//---------------------------------------------------------------------------
//! @file   FilterLensFlare.cpp
//! @brief  レンズフレアフィルター
//---------------------------------------------------------------------------
#include "FilterLensFlare.h"

//! @brief 初期化
//! @return 初期化済み
bool FilterLensFlare::Init()
{
    SetName("FilterLensFlare");

    texture_lens_color = std::make_shared<Texture>("data/Shader/Texture/lens_color.png");

    // ワークテクスチャを作成
    int w = WINDOW_W / 2;
    int h = WINDOW_H / 2;

    auto dxgi_format = DXGI_FORMAT_R11G11B10_FLOAT;
    texture_work_    = std::make_shared<Texture>(w, h, dxgi_format);

    // 縮小テクスチャ作成
    for(int i = 0; i < REDUCTION_MAX_; i++)
    {
        w                        /= 2;
        h                        /= 2;
        texture_reduction_[i][0]  = std::make_shared<Texture>(w, h, dxgi_format);
        texture_reduction_[i][1]  = std::make_shared<Texture>(w, h, dxgi_format);
    }

    // シェーダー初期化
    shader_ps_nd_                   = std::make_shared<ShaderPs>("data/Shader/ps_neutral_density");
    shader_ps_gaussian_             = std::make_shared<ShaderPs>("data/Shader/ps_gaussian");
    shader_ps_                      = std::make_shared<ShaderPs>("data/Shader/ps_lens_flare");
    shader_ps_chromatic_aberration_ = std::make_shared<ShaderPs>("data/Shader/ps_chromatic_aberration.fx");

    buffer_cb_ = CreateShaderConstantBuffer(sizeof(FilterInfo));

    auto execute = [this]() {
        // HDRバッファーから減光フィルターをコピー
        CopyToRenderTarget(texture_work_.get(), GetHdrBuffer(), *shader_ps_nd_);

        SetShaderConstantBuffer(buffer_cb_, DX_SHADERTYPE_PIXEL, 13);

        Texture* mix_texture = texture_work_.get();

        for(int i = 0; i < REDUCTION_MAX_; i++)
        {
            auto& work = texture_reduction_[i];

            CopyToRenderTarget(work[0].get(), mix_texture);

            float inv_screen_w = 1.0f / work[0]->width();    // 1ピクセルのU幅
            float inv_screen_h = 1.0f / work[0]->height();   // 1ピクセルのV高さ

            // ガウシアンブラー適用(縦)
            {
                auto* p          = reinterpret_cast<FilterInfo*>(GetBufferShaderConstantBuffer(buffer_cb_));
                p->pixel_offset_ = {0.0f, inv_screen_h};

                UpdateShaderConstantBuffer(buffer_cb_);

                CopyToRenderTarget(work[1].get(), work[0].get(), *shader_ps_gaussian_);
            }

            // ガウシアンブラー適用(横)
            {
                auto* p          = reinterpret_cast<FilterInfo*>(GetBufferShaderConstantBuffer(buffer_cb_));
                p->pixel_offset_ = {inv_screen_w, 0.0f};

                UpdateShaderConstantBuffer(buffer_cb_);

                CopyToRenderTarget(work[0].get(), work[1].get(), *shader_ps_gaussian_);
            }

            mix_texture = work[0].get();
        }

        SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 13);

        // レンズフレアを生成
        {
            for(int i = 0; i < REDUCTION_MAX_; i++)
            {
                SetTexture(30 + i, texture_reduction_[i][0].get());
            }
            SetTexture(35, texture_lens_color.get());

            // 描画
            CopyToRenderTarget(texture_work_.get(), nullptr, *shader_ps_);

            for(int i = 0; i < REDUCTION_MAX_; i++)
            {
                SetTexture(30 + i, nullptr);
            }
            SetTexture(35, nullptr);
        }

        // 色収差を適用
        {
            // 加算合成
            DxLib::SetDrawBlendMode(DX_BLENDMODE_ADD, 255);

            // 描画
            CopyToRenderTarget(GetHdrBuffer(), texture_work_.get(), *shader_ps_chromatic_aberration_);

            // 元に戻す
            DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
        }

        SetRenderTarget(GetHdrBuffer(), GetDepthStencil());
    };

    SetProc("LensFlare", execute, ProcTiming::Filter, static_cast<ProcPriority>(4));

    return Super::Init();
}

//! @brief 終了
void FilterLensFlare::Exit()
{
    Super::Exit();
    shader_ps_          = nullptr;
    shader_ps_nd_       = nullptr;
    shader_ps_gaussian_ = nullptr;

    DeleteShaderConstantBuffer(buffer_cb_);
    buffer_cb_ = -1;
}

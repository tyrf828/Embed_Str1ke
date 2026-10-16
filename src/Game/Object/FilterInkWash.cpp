//---------------------------------------------------------------------------
//! @file   FilterInkWash.cpp
//! @brief  水墨フィルター
//---------------------------------------------------------------------------
#include "FilterInkWash.h"
#include <Game/Object/Gbuffer.h>

//! @brief 初期化
//! @return 初期化済み
bool FilterInkWash::Init()
{
    SetName("FilterInkWash");

    // ワークテクスチャを作成
    int w = WINDOW_W;
    int h = WINDOW_H;

    auto dxgi_format = DXGI_FORMAT_R11G11B10_FLOAT;
    for(int i = 0; i < 2; i++)
    {
        texture_work_[i] = std::make_shared<Texture>(w, h, dxgi_format);
    }

    shader_ps_color_    = std::make_shared<ShaderPs>("data/Shader/ps_ink_wash_color");
    shader_ps_gaussian_ = std::make_shared<ShaderPs>("data/Shader/ps_gaussian");
    shader_ps_          = std::make_shared<ShaderPs>("data/Shader/ps_filter_ink_wash");
    buffer_cb_          = CreateShaderConstantBuffer(sizeof(InkWashInfo));
    gaussian_cb_        = CreateShaderConstantBuffer(sizeof(BlurInfo));

    texture_noise_ = std::make_shared<Texture>("data/Shader/Noise/ink_noise_texture_3.png");

    auto execute = [this]() {
        auto gbuffer = Scene::Object::Get<GBuffer>("GBuffer");

        if(!gbuffer)
        {
            return;
        }

        auto* info        = static_cast<InkWashInfo*>(GetBufferShaderConstantBuffer(buffer_cb_));
        info_.uv_pixel_.x = 1.0f / WINDOW_W;
        info_.uv_pixel_.y = 1.0f / WINDOW_H;
        *info             = info_;
        UpdateShaderConstantBuffer(buffer_cb_);

        CopyToRenderTarget(texture_work_[0].get(), GetHdrBuffer(), *shader_ps_color_);

        SetShaderConstantBuffer(gaussian_cb_, DX_SHADERTYPE_PIXEL, 13);
        {
            // ガウシアンブラー適用(縦)
            {
                auto* p          = static_cast<BlurInfo*>(GetBufferShaderConstantBuffer(gaussian_cb_));
                p->pixel_offset_ = {0.0f, info_.uv_pixel_.y};

                UpdateShaderConstantBuffer(gaussian_cb_);

                CopyToRenderTarget(texture_work_[1].get(), texture_work_[0].get(), *shader_ps_gaussian_);
            }

            // ガウシアンブラー適用(横)
            {
                auto* p          = static_cast<BlurInfo*>(GetBufferShaderConstantBuffer(gaussian_cb_));
                p->pixel_offset_ = {info_.uv_pixel_.x, 0.0f};

                UpdateShaderConstantBuffer(gaussian_cb_);

                CopyToRenderTarget(texture_work_[0].get(), texture_work_[1].get(), *shader_ps_gaussian_);
            }
        }
        SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 13);

        SetShaderConstantBuffer(buffer_cb_, DX_SHADERTYPE_PIXEL, 13);
        SetTexture(24, texture_noise_.get());
        SetTextureAddressMode(DX_TEXADDRESS_WRAP, 14);
        SetTexture(0, texture_work_[0].get());

        FillRenderTarget(*shader_ps_);
        SetTexture(0, nullptr);

        SetTextureAddressMode(DX_TEXADDRESS_CLAMP, 14);

        SetTexture(24, nullptr);
        SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 13);

        SetRenderTarget(GetHdrBuffer(), GetDepthStencil());
    };

    SetProc("InkWash", execute, ProcTiming::Filter, static_cast<ProcPriority>(10));

    return Super::Init();
}

//! @brief 終了
void FilterInkWash::Exit()
{
    Super::Exit();

    DeleteShaderConstantBuffer(buffer_cb_);
    DeleteShaderConstantBuffer(gaussian_cb_);

    buffer_cb_   = -1;
    gaussian_cb_ = -1;

    shader_ps_color_.reset();
    shader_ps_gaussian_.reset();
    shader_ps_.reset();
    texture_noise_.reset();
    texture_work_.fill(nullptr);
}

//! @brief GUI
void FilterInkWash::GUI()
{
    Super::GUI();
    if(ImGui::Begin(GetName().data()))
    {
        ImGui::SliderFloat("Edge width (pixels)", &info_.edge_width_, 0.0f, 8.0f);
        ImGui::SliderFloat("Edge threshold", &info_.sensitivity_, 0.0f, 1.0f);
        ImGui::SliderFloat("World noise scale", &info_.noise_scale_, 0.01f, 2.0f);
    }
    ImGui::End();
}

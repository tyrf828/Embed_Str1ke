//---------------------------------------------------------------------------
//! @file   Gbuffer.cpp
//! @brief  GBuffer管理
//---------------------------------------------------------------------------
#include "Gbuffer.h"

//! @brief 初期化
//! @return 初期化済み
bool GBuffer::Init()
{
    SetName("GBuffer");

    // GBufferテクスチャを生成
    int width  = WINDOW_W;
    int height = WINDOW_H;

    gbuffer_texture_[0] = std::make_shared<Texture>(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
    gbuffer_texture_[1] = std::make_shared<Texture>(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
    gbuffer_texture_[2] = std::make_shared<Texture>(width, height, DXGI_FORMAT_R16G16_FLOAT);
    gbuffer_texture_[3] = std::make_shared<Texture>(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);
    depth_texture_      = std::make_shared<Texture>(width, height, DXGI_FORMAT_R32_FLOAT);

    // GBuffer開始描画
    {
        auto begin = [this]() {
            // GBufferをMRTで設定
            std::array<Texture*, GBUFFER_NUM> render_targets;
            render_targets[0] = gbuffer_texture_[0].get();
            render_targets[1] = gbuffer_texture_[1].get();
            render_targets[2] = gbuffer_texture_[2].get();
            render_targets[3] = gbuffer_texture_[3].get();

            SetRenderTarget(GBUFFER_NUM, render_targets.data(), GetDepthStencil());

            // GBufferをクリア
            ClearColor(gbuffer_texture_[0].get(), float4(0.0f, 0.0f, 0.0f, 0.0f));
            ClearColor(gbuffer_texture_[1].get(), float4(0.0f, 0.0f, 0.0f, 0.0f));
            ClearColor(gbuffer_texture_[2].get(), float4(0.0f, 0.0f, 0.0f, 0.0f));
            ClearColor(gbuffer_texture_[3].get(), float4(0.0f, 0.0f, 0.0f, 0.0f));

            // デプス値をクリア
            ClearDepth(GetDepthStencil(), 1.0f);

            // シーン用カメラを設定
            if(auto camera = Scene::GetCurrentCamera().lock())
            {
                camera->SetCameraTransform();
            }
        };

        SetProc("GBuffer::begin", begin, ProcTiming::Gbuffer, static_cast<ProcPriority>(0));
    }

    // GBuffer終了描画
    {
        auto end = [this]() {
            // デプスバッファを一旦外す
            SetRenderTarget(GetHdrBuffer(), nullptr);

            // コピーしてDepthバッファをカラーテクスチャに保存
            {
                GetD3DDeviceContext()->CopyResource(depth_texture_->d3dResource(), GetDepthStencil()->d3dResource());
            }

            // レンダリングターゲットをデフォルトに戻す(MRT off)
            SetRenderTarget(GetHdrBuffer(), GetDepthStencil());

            // GBufferをテクスチャに設定する
            SetGBufferTextures();
        };

        SetProc("GBuffer::end", end, ProcTiming::Gbuffer, static_cast<ProcPriority>(65535));
    }

    return Super::Init();
}

//! @brief 終了
void GBuffer::Exit()
{
    Super::Exit();
    for(int i = 0; i < 5; i++)
    {
        SetTexture(16 + i, nullptr);
    }
    gbuffer_texture_.fill(nullptr);
    depth_texture_ = nullptr;
}

//! @brief GUI描画
void GBuffer::GUI()
{
    Super::GUI();

    ImGui::Begin(GetName().data());
    {
        ImGui::Separator();

        for(int i = 0; i < 4; i++)
        {
            // バッファ表示
            ImGui::Image(gbuffer_texture_[i]->srv(),                                // バッファテクスチャ
                         ImVec2((float)WINDOW_W * 0.5f, (float)WINDOW_H * 0.5f));   // 画面上のサイズ
        }
    }
    ImGui::End();
}

//! @brief GBufferのテクスチャを取得します
//! @param index インデックス
//! @return テクスチャ
Texture* GBuffer::GetGBuffer(int index) const
{
    if(gbuffer_texture_.size() <= index)
    {
        return nullptr;
    }
    return gbuffer_texture_[index].get();
}

//! @brief GBufferのテクスチャを設定します
void GBuffer::SetGBufferTextures()
{
    SetTexture(16, gbuffer_texture_[0].get());
    SetTexture(17, gbuffer_texture_[1].get());
    SetTexture(18, gbuffer_texture_[2].get());
    SetTexture(19, gbuffer_texture_[3].get());
    SetTexture(20, depth_texture_.get());   // デプスバッファをコピーしたカラーバッファ
}

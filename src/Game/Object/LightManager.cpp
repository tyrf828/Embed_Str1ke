//---------------------------------------------------------------------------
//! @file   LightManager.cpp
//! @brief  Light管理
//---------------------------------------------------------------------------
#include "LightManager.h"

//! @brief 初期化
//! @return 初期化済み
bool LightManager::Init()
{
    SetName("LightManager");

    // Light Accumulation テクスチャを生成
    light_texture_[0] = std::make_shared<Texture>(WINDOW_W, WINDOW_H, DXGI_FORMAT_R11G11B10_FLOAT);   // Diffuse
    light_texture_[1] = std::make_shared<Texture>(WINDOW_W, WINDOW_H, DXGI_FORMAT_R11G11B10_FLOAT);   // Specular

    // シェーダー初期化
    shader_ps_ = std::make_shared<ShaderPs>("data/Shader/ps_light_finish");

    // Light Accumulation開始描画
    {
        auto begin = [this]() {
            // Light AccumulationをMRTで設定
            std::array<Texture*, 2> render_targets;
            render_targets[0] = light_texture_[0].get();
            render_targets[1] = light_texture_[1].get();

            SetRenderTarget(2, render_targets.data());

            // Light Accumulationをクリア
            ClearColor(light_texture_[0].get(), float4(0.0f, 0.0f, 0.0f, 0.0f));
            ClearColor(light_texture_[1].get(), float4(0.0f, 0.0f, 0.0f, 0.0f));

            // シーン用カメラを設定
            if(auto camera = Scene::GetCurrentCamera().lock())
            {
                camera->SetCameraTransform();
            }
        };

        // ProcPriority 0は FilterSSAOが GbufferとLightの間で動作している
        SetProc("LightManager::begin", begin, ProcTiming::Light, static_cast<ProcPriority>(2));
    }

    // Light Accumulation終了描画
    {
        auto end = [this]() {
            // レンダリングターゲットをデフォルトに戻す(MRT off)
            SetRenderTarget(GetHdrBuffer(), GetDepthStencil());

            // DiffuseとSpecularをHDRバッファに合成して出力
            // Light Accumulationをテクスチャとしてアクセスできるようにする
            SetTexture(21, light_texture_[0].get());   // Diffuse
            SetTexture(22, light_texture_[1].get());   // Specular

            // 合成描画
            FillRenderTarget(*shader_ps_);

            // もとに戻す
            SetTexture(21, nullptr);
            SetTexture(22, nullptr);
        };

        SetProc("LightManager::end", end, ProcTiming::Light, static_cast<ProcPriority>(65535));
    }

    return Super::Init();
}

//! @brief 終了
void LightManager::Exit()
{
    Super::Exit();
    shader_ps_.reset();

    light_texture_[0].reset();
    light_texture_[1].reset();
}

//! @brief GUI描画
void LightManager::GUI()
{
    Super::GUI();

    ImGui::Begin(GetName().data());
    {
        ImGui::Separator();

        for(int i = 0; i < 2; i++)
        {
            // DiffuseとSpecularバッファ表示
            ImGui::Image(light_texture_[i]->srv(), ImVec2(float(WINDOW_W / 2), float(WINDOW_H / 2)));
        }
    }
    ImGui::End();
}

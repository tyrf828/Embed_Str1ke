//---------------------------------------------------------------------------
//! @file   SSAO.cpp
//! @brief  Screen Space Amibient Occlusion
//---------------------------------------------------------------------------
#include "SSAO.h"
#include "Game/Object/Gbuffer.h"

//! @brief 初期化
//! @return 初期化済み
bool SSAO::Init()
{
    SetName("SSAO");

    // シェーダー初期化
    shader_ps_ = std::make_shared<ShaderPs>("data/Shader/ps_ssao");

    {
        auto execute = [this]() {
            auto gbuffer = Scene::Object::Get<GBuffer>();
            if(!gbuffer)
            {
                return;
            }

            // AOを含むGBufferのテクスチャを外す (slot=16)
            SetTexture(16, nullptr);

            // GBufferの一部のAOを含むGBufferをRenderTargetにする
            SetRenderTarget(gbuffer->GetGBuffer(0));

            {
                // シェーダー側でAO成分を出力して、合成する
                SetDrawCustomBlendMode(true, DX_BLEND_ONE, DX_BLEND_ONE, DX_BLENDOP_MIX, DX_BLEND_ONE, DX_BLEND_ONE, DX_BLENDOP_MIX, 255);

                // 全画面にシェーダーを適用
                FillRenderTarget(*shader_ps_);

                SetDrawBlendMode(DX_BLENDMODE_NOBLEND, -1);
            }

            // RenderTargetからGBuffer[0]を外す
            SetRenderTarget(nullptr);

            // GBufferを元に戻す
            gbuffer->SetGBufferTextures();
        };

        // SSAO は GBuffer::end() と Light::begin() の間に動作する
        SetProc("SSAO", execute, ProcTiming::Light, static_cast<ProcPriority>(0));
    }

    return Super::Init();
}

//! @brief 終了
void SSAO::Exit()
{
    Super::Exit();
    shader_ps_ = nullptr;
}

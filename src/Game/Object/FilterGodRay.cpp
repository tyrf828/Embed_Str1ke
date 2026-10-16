//---------------------------------------------------------------------------
//! @file   FilterGodRay.cpp
//! @brief  ゴッドレイフィルター
//---------------------------------------------------------------------------
#include "FilterGodRay.h"
#include <Game/Object/Sun.h>
#include <System/Component/ComponentCamera.h>

//! @brief 初期化
//! @return 初期化済み
bool FilterGodRay::Init()
{
    SetName("FilterGodRay");

    // 縮小テクスチャ作成
    const int width  = WINDOW_W / 2;
    const int height = WINDOW_H / 2;

    const auto format = DXGI_FORMAT_R11G11B10_FLOAT;
    texture_work_     = std::make_shared<Texture>(width, height, format);

    shader_ps_ = std::make_shared<ShaderPs>("data/Shader/ps_god_ray");
    buffer_cb_ = CreateShaderConstantBuffer(sizeof(GodRayInfo));

    auto execute = [this]() {
        auto sun = Scene::Object::Get<Sun>("Sun");

        if(!sun)
        {
            return;
        }

        auto camera = Scene::GetCurrentCamera().lock();
        if(!camera)
        {
            return;
        }

        float3 to_light     = normalize(sun->GetTranslate() - camera->GetOwnerPtr()->GetTranslate());
        float3 camera_front = normalize(camera->GetOwnerPtr()->GetVectorAxisZ());
        float light_dot     = dot(camera_front, to_light);
        if(light_dot < 0.0f)
        {
            return;
        }
        VECTOR light_screen = ConvWorldPosToScreenPos(cast(sun->GetTranslate()));

        auto* info = reinterpret_cast<GodRayInfo*>(GetBufferShaderConstantBuffer(buffer_cb_));

        info->light_pos_ = {light_screen.x / (float)WINDOW_W, light_screen.y / (float)WINDOW_H};

        UpdateShaderConstantBuffer(buffer_cb_);
        SetShaderConstantBuffer(buffer_cb_, DX_SHADERTYPE_PIXEL, 13);

        // HDRバッファーからゴッドレイを合成する
        CopyToRenderTarget(texture_work_.get(), GetHdrBuffer(), *shader_ps_);

        SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 13);

        // HDRへ加算合成する
        SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
        CopyToRenderTarget(GetHdrBuffer(), texture_work_.get());
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, -1);

        SetRenderTarget(GetHdrBuffer(), GetDepthStencil());
    };

    SetProc("GodRay", execute, ProcTiming::Filter, static_cast<ProcPriority>(2));

    return Super::Init();
}

//! @brief 終了
void FilterGodRay::Exit()
{
    Super::Exit();

    shader_ps_    = nullptr;
    texture_work_ = nullptr;

    DeleteShaderConstantBuffer(buffer_cb_);
    buffer_cb_ = -1;
}

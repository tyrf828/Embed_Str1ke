//---------------------------------------------------------------------------
//! @file   Decal.cpp
//! @brief  デカールコンポーネント
//---------------------------------------------------------------------------
#include <Game/Component/Decal.h>

#include <System/Graphics/Texture.h>

//! @brief 初期化
void Decal::Init()
{
    Super::Init();
    shader_ps_ = std::make_shared<ShaderPs>("data/Shader/ps_decal.fx");

    decal_cb_ = CreateShaderConstantBuffer(sizeof(DecalInfo));
}

//! @brief 描画
void Decal::Draw()
{
    Super::Draw();

    auto* p               = reinterpret_cast<DecalInfo*>(GetBufferShaderConstantBuffer(decal_cb_));
    p->mat_inverse_world_ = inverse(GetOwner()->GetWorldMatrix());
    p->alpha_             = alpha_;
    {
        UpdateShaderConstantBuffer(decal_cb_);
    }

    SetShaderConstantBuffer(decal_cb_, DX_SHADERTYPE_PIXEL, 5);

    // 2Dの描画
    {
        VERTEX2DSHADER v[4]{};

        // 頂点データの準備
        v[0].pos = {0.0f, 0.0f, 0.0f};
        v[1].pos = {float(WINDOW_W), 0.0f, 0.0f};
        v[2].pos = {0.0f, float(WINDOW_H), 0.0f};
        v[3].pos = {float(WINDOW_W), float(WINDOW_H), 0.0f};
        v[0].rhw = 1.0f;
        v[1].rhw = 1.0f;
        v[2].rhw = 1.0f;
        v[3].rhw = 1.0f;
        v[0].dif = GetColorU8(255, 255, 255, 255);
        v[1].dif = GetColorU8(255, 255, 255, 255);
        v[2].dif = GetColorU8(255, 255, 255, 255);
        v[3].dif = GetColorU8(255, 255, 255, 255);
        v[0].u   = 0.0f;
        v[0].v   = 0.0f;
        v[1].u   = 1.0f;
        v[1].v   = 0.0f;
        v[2].u   = 0.0f;
        v[2].v   = 1.0f;
        v[3].u   = 1.0f;
        v[3].v   = 1.0f;

        if(!decal_texture_)
        {
            return;
        }

        // 使用するテクスチャを設定
        SetTexture(36, decal_texture_.get());

        // 使用するピクセルシェーダーを設定
        SetUsePixelShader(*shader_ps_);

        // 描画
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

        DrawPrimitive2DToShader(v, 4, DX_PRIMTYPE_TRIANGLESTRIP);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, -1);
    }
    SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 5);

    // デバッグのボックス描画
    if(draw_debug_box)
    {
        float3 pos   = GetOwner()->GetTranslate();
        float width  = GetOwner()->GetScaleAxisXYZ().x * 0.5f;
        float height = GetOwner()->GetScaleAxisXYZ().y * 0.5f;
        float depth  = GetOwner()->GetScaleAxisXYZ().z * 0.5f;
        float3 right = normalize(GetOwner()->GetVectorAxisX());
        float3 up    = normalize(GetOwner()->GetVectorAxisY());
        float3 front = normalize(GetOwner()->GetVectorAxisZ());

        DrawLine3D(cast(pos - right * width - up * height - front * depth), cast(pos - right * width - up * height + front * depth), GetColor(255, 255, 255));

        DrawLine3D(cast(pos - right * width - up * height - front * depth), cast(pos - right * width + up * height - front * depth), GetColor(255, 255, 255));

        DrawLine3D(cast(pos - right * width - up * height - front * depth), cast(pos + right * width - up * height - front * depth), GetColor(255, 255, 255));

        DrawLine3D(cast(pos + right * width - up * height - front * depth), cast(pos + right * width + up * height - front * depth), GetColor(255, 255, 255));

        DrawLine3D(cast(pos + right * width - up * height - front * depth), cast(pos + right * width - up * height + front * depth), GetColor(255, 255, 255));

        DrawLine3D(cast(pos - right * width - up * height + front * depth), cast(pos - right * width + up * height + front * depth), GetColor(255, 255, 255));

        DrawLine3D(cast(pos - right * width - up * height + front * depth), cast(pos + right * width - up * height + front * depth), GetColor(255, 255, 255));

        DrawLine3D(cast(pos - right * width + up * height - front * depth), cast(pos - right * width + up * height + front * depth), GetColor(255, 255, 255));

        DrawLine3D(cast(pos - right * width + up * height - front * depth), cast(pos + right * width + up * height - front * depth), GetColor(255, 255, 255));

        DrawLine3D(cast(pos + right * width - up * height + front * depth), cast(pos + right * width + up * height + front * depth), GetColor(255, 255, 255));

        DrawLine3D(cast(pos - right * width + up * height + front * depth), cast(pos + right * width + up * height + front * depth), GetColor(255, 255, 255));

        DrawLine3D(cast(pos + right * width + up * height - front * depth), cast(pos + right * width + up * height + front * depth), GetColor(255, 255, 255));
    }
}

//! @brief 終了
void Decal::Exit()
{
    Super::Exit();
    shader_ps_.reset();
    decal_texture_.reset();

    DeleteShaderConstantBuffer(decal_cb_);
    decal_cb_ = -1;
}

//! @brief GUI描画
void Decal::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    ImGui::Separator();
    if(ImGui::TreeNode(u8"デカール"))
    {
        if(ImGui::Button(u8"削除"))
        {
            GetOwner()->RemoveComponent(shared_from_this());
        }

        ImGui::Checkbox(u8"デバック描画", &draw_debug_box);
        ImGui::SliderFloat(u8"透明度", &alpha_, 0.0f, 1.0f);

        ImGui::TreePop();
    }
    ImGui::End();
}

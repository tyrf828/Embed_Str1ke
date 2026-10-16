//===========================================================================
//! @file DissolveEffect.cpp
//! @brief Dissolve エフェクトコンポーネント
//===========================================================================
#include "DissolveEffect.h"
#include <System/Component/ComponentModel.h>

//! @brief 初期化
void DissolveEffect::Init()
{
    Super::Init();
    auto model = GetOwner()->GetComponent<ComponentModel>();
    if(!model)
    {
        return;
    }
    shader_ps_ = std::make_shared<ShaderPs>("data/Shader/ps_dissolve");

    noise_texture_ = std::make_shared<Texture>("data/Shader/Noise/burnmap.dds");

    dissolve_info_cb_ = CreateShaderConstantBuffer(sizeof(DissolveInfo));
}

//! @brief 更新
void DissolveEffect::Update()
{
    Super::Update();

    if(reverse_)
    {
        if(dissolve_amount_ > -0.1f)
        {
            // トランジション進行
            dissolve_amount_ -= GetDeltaTime() * animation_speed_;
            dissolve_amount_  = std::max(dissolve_amount_, -0.1f);
            if(dissolve_amount_ <= -0.1f && auto_destroy_)
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
        }
    }
    else
    {
        if(dissolve_amount_ < 1.0f)
        {
            // トランジション進行
            dissolve_amount_ += GetDeltaTime() * animation_speed_;
            dissolve_amount_  = std::min(dissolve_amount_, 1.0f);
            if(dissolve_amount_ >= 1.0f && auto_destroy_)
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
        }
    }
}

//! @brief 描画
void DissolveEffect::Draw()
{
    Super::Draw();

    auto* p = reinterpret_cast<DissolveInfo*>(GetBufferShaderConstantBuffer(dissolve_info_cb_));
    if(p)
    {
        p->dissolve_amount_ = dissolve_amount_;
        // 定数バッファ更新
        UpdateShaderConstantBuffer(dissolve_info_cb_);
    }
    // 定数バッファを設定
    SetShaderConstantBuffer(dissolve_info_cb_, DX_SHADERTYPE_PIXEL, 8);
    // ノイズテクスチャを設定
    SetTexture(11, noise_texture_.get());
    if(auto model = GetOwner()->GetComponent<ComponentModel>())
    {
        model->SetShader(nullptr, shader_ps_.get());
    }
    SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 8);
}

//! @brief 終了
void DissolveEffect::Exit()
{
    Super::Exit();

    auto model = GetOwner()->GetComponent<ComponentModel>();
    if(model)
    {
        model->SetShader(nullptr, nullptr);
    }

    if(dissolve_info_cb_ != -1)
    {
        DeleteShaderConstantBuffer(dissolve_info_cb_);
        dissolve_info_cb_ = -1;
    }

    SetTexture(11, nullptr);

    shader_ps_.reset();
    noise_texture_.reset();
}

//! @brief GUI描画
void DissolveEffect::GUI()
{
    Super::GUI();

    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("DissolveEffect"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            if(ImGui::Button(UNIQUE_TEXT("Start Dissolve")))
            {
                StartDissolve();
            }

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

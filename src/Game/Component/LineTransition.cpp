//===========================================================================
//! @file LineTransition.cpp
//! @brief シーン転移(ラインエフェクト)
//===========================================================================
#include "LineTransition.h"
#include <System/Graphics/Render.h>

//! @brief 初期化
void LineTransition::Init()
{
    Super::Init();

    // ピクセルシェーダー
    shader_ps_ = std::make_shared<ShaderPs>("data/Shader/ps_line_transition");
    // 定数バッファを作成
    parameter_cb_ = CreateShaderConstantBuffer(sizeof(TransitionInfo));
    texture_      = std::make_shared<Texture>(WINDOW_W, WINDOW_H, DXGI_FORMAT_R8G8B8A8_UNORM);

    auto draw = [&]() {
        auto target_desc   = GetRenderTarget();
        auto render_target = target_desc.color_targets_[0];

        // 現在の画面をワークへコピー
        CopyToRenderTarget(texture_.get(), render_target);

        // 定数バッファ更新
        auto* p = reinterpret_cast<TransitionInfo*>(GetBufferShaderConstantBuffer(parameter_cb_));
        {
            p->transition_progress_ = transition_progress_;
            p->blend_ratio_         = blend_ratio_;

            UpdateShaderConstantBuffer(parameter_cb_);
        }

        // 定数バッファをb9に設定
        SetShaderConstantBuffer(parameter_cb_, DX_SHADERTYPE_PIXEL, 9);

        SetWriteZBufferFlag(false);
        CopyToRenderTarget(render_target, texture_.get(), *shader_ps_);
        SetWriteZBufferFlag(true);
        SetRenderTarget(target_desc);
    };

    // 描画タイミングを設定して描画
    SetProc("LineTransitionDraw", draw, ProcTiming::PostDraw, static_cast<ProcPriority>(65535));
}

//! @brief 更新
void LineTransition::Update()
{
    Super::Update();

    if(finish_transition_)
    {
        return;
    }

    if(start_transition_)
    {
        // トランジション進行
        transition_progress_ += transition_speed_ * GetDeltaTime();
        blend_ratio_         -= transition_speed_ * 0.5f * GetDeltaTime();

        if(transition_progress_ >= 1.0f)
        {
            transition_progress_ = 1.0f;
            blend_ratio_         = 0.0f;
            finish_transition_   = true;
            start_transition_    = false;
        }
    }
}

//! @brief 終了
void LineTransition::Exit()
{
    Super::Exit();

    DeleteShaderConstantBuffer(parameter_cb_);
    parameter_cb_ = -1;
    shader_ps_.reset();
}

//! @brief GUI描画
void LineTransition::GUI()
{
    Super::GUI();

    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("LineTransition"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }

            ImGui::Checkbox(UNIQUE_TEXT(u8"開始"), &start_transition_);
            if(ImGui::Button(UNIQUE_TEXT(u8"リセット")))
            {
                ResetTransition();
            }

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

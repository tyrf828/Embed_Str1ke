//---------------------------------------------------------------------------
//! @file   LightPoint.cpp
//! @brief  点光源
//---------------------------------------------------------------------------
#include "LightPoint.h"

//! @brief 初期化
//! @return 初期化済み
bool LightPoint::Init()
{
    SetName("LightPoint");

    // シェーダー初期化
    shader_ps_ = std::make_shared<ShaderPs>("data/Shader/ps_light_point.fx");

    // 定数バッファを作成
    buffer_cb_ = DxLib::CreateShaderConstantBuffer(sizeof(PointLightInfo));

    {
        auto execute = [this]() {
            // 定数バッファを更新
            void* p               = GetBufferShaderConstantBuffer(buffer_cb_);
            info_.light_position_ = GetTranslate();
            *(PointLightInfo*)p   = info_;

            UpdateShaderConstantBuffer(buffer_cb_);   // GPUへ転送

            // 定数バッファを設定
            SetShaderConstantBuffer(buffer_cb_, DX_SHADERTYPE_PIXEL, 3);

            SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
            {
                // 全画面にシェーダーを適用
                FillRenderTarget(*shader_ps_);
            }
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, -1);

            // シーン用カメラを設定
            if(auto camera = Scene::GetCurrentCamera().lock())
            {
                matrix mat_view_proj = mul(camera->GetViewMatrix(), camera->GetProjMatrix());
            }
        };

        auto executeDebug = [this]() {
            // デバッグ球を描画
            SetUseLighting(false);
            DrawSphere3D(cast(info_.light_position_), info_.light_radius_, 16, GetColor(0, 255, 255), GetColor(0, 0, 0), false);

            SetUseLighting(true);
        };

        SetProc("LightPoint", execute, ProcTiming::Light, static_cast<ProcPriority>(32768));
    }

    return Super::Init();
}

//! @brief 終了
void LightPoint::Exit()
{
    // 定数バッファを解放
    if(buffer_cb_ != -1)
    {
        DeleteShaderConstantBuffer(buffer_cb_);
        buffer_cb_ = -1;
    }

    Super::Exit();
}

//! @brief GUI
void LightPoint::GUI()
{
    Super::GUI();

    ImGui::Begin(GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode(u8"点光源パラメータ"))
        {
            // 光源のカラー
            ImGui::ColorEdit4(u8"光源カラー", (float*)&info_.light_color_);

            // 光源の強度
            ImGui::DragFloat(u8"光源強度", &info_.light_intensity_, 0.1f, 0.0f, 100.0f, "%.2f");

            // 光源の影響範囲半径
            ImGui::DragFloat(u8"影響範囲半径", &info_.light_radius_, 1.0f, 0.0f, 1000.0f, "%.2f");

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

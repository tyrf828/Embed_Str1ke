//---------------------------------------------------------------------------
//! @file   LightDirectional.cpp
//! @brief  平行光源
//---------------------------------------------------------------------------
#include "LightDirectional.h"

//! @brief 初期化
//! @return 初期化済み
bool LightDirectional::Init()
{
    SetName("LightDirectional");

    // シェーダー初期化
    shader_ps_ = std::make_shared<ShaderPs>("data/Shader/ps_light_directional");

    {
        auto execute = [this]() {
            SetDrawBlendMode(DX_BLENDMODE_ADD, 255);

            // 全画面にシェーダーを適用
            FillRenderTarget(*shader_ps_);

            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, -1);
        };

        SetProc("LightDirectional", execute, ProcTiming::Light, static_cast<ProcPriority>(32768));
    }

    return Super::Init();
}

//! @brief 終了
inline void LightDirectional::Exit()
{
    Super::Exit();
    shader_ps_.reset();
}

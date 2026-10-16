//====================================================================
//! @file   SlashEffect.cpp
//! @brief  画面分割分割エフェクト
//====================================================================
#include "SlashEffect.h"

//! @brief 初期化
//! @return 初期化済み
bool SlashEffect::Init()
{
    Super::Init();

    // シェーダー作成
    shader_ps_ = std::make_shared<ShaderPs>("data/Shader/ps_slash");

    // 描画用テクスチャ作成
    texture_ = std::make_shared<Texture>(WINDOW_W, WINDOW_H, DXGI_FORMAT_R8G8B8A8_UNORM);

    // 定数バッファ作成
    info_cb_ = CreateShaderConstantBuffer(sizeof(Info));

    zoom_ = ZOOM_MAX_;

    // 描画処理登録
    auto draw = [&]() {
        auto target_desc   = GetRenderTarget();
        auto render_target = target_desc.color_targets_[0];

        // 現在の画面をワークへコピー
        CopyToRenderTarget(texture_.get(), render_target);

        // 定数バッファ更新
        auto* p  = reinterpret_cast<Info*>(GetBufferShaderConstantBuffer(info_cb_));
        p->zoom_ = zoom_;
        UpdateShaderConstantBuffer(info_cb_);

        SetShaderConstantBuffer(info_cb_, DX_SHADERTYPE_PIXEL, 7);

        // シェーダー適用して戻す
        SetWriteZBufferFlag(false);
        CopyToRenderTarget(render_target, texture_.get(), *shader_ps_);
        SetWriteZBufferFlag(true);

        SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 7);

        SetRenderTarget(target_desc);
    };

    SetProc("ScreenSlash", draw, ProcTiming::Filter, ProcPriority::NORMAL);

    return true;
}

void SlashEffect::Update()
{
    Super::Update();

    // 拡大率を普通の画面サイズまで戻ったら削除
    if(zoom_ <= 1.0f)
    {
        zoom_ = 1.0f;
        Scene::Object::Release(shared_from_this());
    }
}

void SlashEffect::Exit()
{
    Super::Exit();
    // 定数バッファ削除
    DeleteShaderConstantBuffer(info_cb_);
    info_cb_ = -1;
    shader_ps_.reset();
}

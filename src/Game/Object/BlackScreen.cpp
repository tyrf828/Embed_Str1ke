//===========================================================================
//! @file BlackScreen.cpp
//! @brief 黒画面のフェード
//===========================================================================
#include "BlackScreen.h"

//! @brief 初期化
//! @return 初期化済み
bool BlackScreen::Init()
{
    Super::Init();

    SetProc("BlackScreen:LateDraw", [this]() { LateDraw(); }, ProcTiming::LateDraw, static_cast<ProcPriority>(65535));

    return true;
}

//! @brief 更新
void BlackScreen::Update()
{
    Super::Update();
    if(!is_start_)
    {
        return;
    }
    // 透明度を上げる
    alpha_rate_ += fade_time_ * GetDeltaTime() * direction_;

    if(alpha_rate_ >= 1.0f || alpha_rate_ <= 0.0f)
    {
        if(auto_destroy_)
        {
            Scene::Object::Release(shared_from_this());
        }
    }
    // 範囲制限
    alpha_rate_ = std::clamp(alpha_rate_, 0.0f, 1.0f);
}

//! @brief 描画
void BlackScreen::LateDraw()
{
    Super::LateDraw();

    // 透明度に応じたブレンド値を計算
    int alpha = (int)(255.0f * alpha_rate_);

    // 黒画面の描画
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
    DrawFillBox(0, 0, WINDOW_W, WINDOW_H, GetColor(0, 0, 0));
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, -1);
}

//! @brief フェードイン開始
//! @param auto_destroy フェード完了後に自動削除するか
void BlackScreen::StartFadeIn(bool auto_destroy)
{
    direction_    = 1;
    alpha_rate_   = 0.0f;
    is_start_     = true;
    auto_destroy_ = auto_destroy;
}

//! @brief フェードアウト開始
//! @param auto_destroy フェード完了後に自動削除するか
void BlackScreen::StartFadeOut(bool auto_destroy)
{
    direction_    = -1;
    alpha_rate_   = 1.0f;
    is_start_     = true;
    auto_destroy_ = auto_destroy;
}

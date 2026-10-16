//===========================================================================
//! @file InkSplash.cpp
//! @brief 水墨の2Dエフェクト
//===========================================================================
#include "InkSplash.h"

//! @brief 初期化
//! @return 初期化済み
bool InkSplash::Init()
{
    Super::Init();

    LoadDivGraph("data/Game/GHandle/ink_splash.png", IMAGE_MAX_, 4, 4, 640, 360, ink_ghandle_.data());
    return true;
}

//! @brief 更新
void InkSplash::Update()
{
    Super::Update();

    // 1枚の時間計算
    const float SINGLE_IMAGE_TIME  = ANIM_TIME_ / (float)IMAGE_MAX_;
    anim_timer_                   += GetDeltaTime();

    // 画像切り替え
    if(anim_timer_ >= SINGLE_IMAGE_TIME)
    {
        anim_timer_ = 0.0f;
        if(now_image_index_ >= IMAGE_MAX_)
        {
            Scene::Object::Release(shared_from_this());
        }
        else
        {
            now_image_index_++;
        }
    }
}

//! @brief 描画
void InkSplash::LateDraw()
{
    Super::LateDraw();

    if(now_image_index_ < IMAGE_MAX_)
    {
        DrawExtendGraph(0, 0, WINDOW_W, WINDOW_H, ink_ghandle_[now_image_index_], TRUE);
    }
}

//! @brief 終了
void InkSplash::Exit()
{
    Super::Exit();

    // ハンドルを削除
    for(int i = 0; i < IMAGE_MAX_; i++)
    {
        DeleteGraph(ink_ghandle_[i]);
    }
}

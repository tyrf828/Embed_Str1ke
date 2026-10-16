//===========================================================================
//! @file Rose2D.cpp
//! @brief バラの2Dエフェクト
//===========================================================================
#include "Rose2D.h"

//! @brief 初期化
//! @return 初期化済み
bool Rose2D::Init()
{
    Super::Init();

    // 画像を読み込み
    LoadDivGraph("data/Game/GHandle/pipo-roseeffect02_192.png", IMAGE_MAX_, 5, 3, 192, 192, rose_ghandle_.data());
    return true;
}

//! @brief 更新
void Rose2D::Update()
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
void Rose2D::LateDraw()
{
    Super::LateDraw();
    if(now_image_index_ < IMAGE_MAX_)
    {
        DrawRotaGraphF(draw_pos_.x, draw_pos_.y, 1.0f * SCALE_WINDOW, 0.0f, rose_ghandle_[now_image_index_], TRUE);
    }
}

//! @brief 終了
void Rose2D::Exit()
{
    Super::Exit();
    // ハンドルを削除
    for(int i = 0; i < IMAGE_MAX_; i++)
    {
        DeleteGraph(rose_ghandle_[i]);
    }
}

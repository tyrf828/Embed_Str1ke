//===========================================================================
//! @file InkSplash.h
//! @brief 水墨の2Dエフェクト
//===========================================================================
#pragma once

USING_PTR(InkSplash);

//---------------------------------------------------------------------------
//! @class InkSplash
//! @brief 水墨の2Dエフェクト
//---------------------------------------------------------------------------
class InkSplash : public Object
{
public:
    BP_OBJECT_DECL(InkSplash, "InkSplash");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 描画
    void LateDraw() override;

    //! @brief 終了
    void Exit() override;

private:
    static constexpr int IMAGE_MAX_ = 16;         //!< 画像枚数
    std::array<int, IMAGE_MAX_> ink_ghandle_{};   //!< 画像ハンドル配列
    static constexpr float ANIM_TIME_ = 0.5f;     //!< アニメーション時間
    float anim_timer_                 = 0.0f;     //!< アニメーションタイマー
    int now_image_index_              = 0;        //!< 現在の画像インデックス
};

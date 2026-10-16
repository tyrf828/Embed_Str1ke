//===========================================================================
//! @file Rose2D.h
//! @brief バラの2Dエフェクト
//===========================================================================
#pragma once

USING_PTR(Rose2D);

//---------------------------------------------------------------------------
//! @class Rose2D
//! @brief バラの2Dエフェクト
//---------------------------------------------------------------------------
class Rose2D : public Object
{
public:
    BP_OBJECT_DECL(Rose2D, "Rose2D");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 描画
    void LateDraw() override;

    //! @brief 終了
    void Exit() override;

    //! @brief 描画位置を設定
    void SetDrawPos(float2 pos)
    {
        draw_pos_ = pos;
    }

private:
    static constexpr int IMAGE_MAX_ = 15;          //!< 画像枚数
    std::array<int, IMAGE_MAX_> rose_ghandle_{};   //!< バラ画像ハンドル配列
    static constexpr float ANIM_TIME_ = 1.0f;      //!< アニメーション時間
    float anim_timer_                 = 0.0f;      //!< アニメーションタイマー
    int now_image_index_              = 0;         //!< 現在の画像インデックス
    float2 draw_pos_                  = 0.0f;      // 描画位置
};

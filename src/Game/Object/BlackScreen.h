#pragma once
//===========================================================================
//! @file BlackScreen.h
//! @brief 黒画面のフェード
//===========================================================================

USING_PTR(BlackScreen);
//---------------------------------------------------------------------------
//! @class BlackScreen
//! @brief 黒画面のフェード
//---------------------------------------------------------------------------
class BlackScreen : public Object
{
public:
    BP_OBJECT_DECL(BlackScreen, u8"BlackScreen");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 描画
    void LateDraw() override;

    //! @brief フェード時間を設定
    //! @param time フェード時間
    //! @return 自身のポインタ
    BlackScreenPtr SetFadeTime(float time)
    {
        fade_time_ = 1.0f / time;
        return std::static_pointer_cast<BlackScreen>(shared_from_this());
    }

    //! @brief フェードイン開始
    //! @param auto_destroy フェード完了後に自動削除するか
    void StartFadeIn(bool auto_destroy = false);

    //! @brief フェードアウト開始
    //! @param auto_destroy フェード完了後に自動削除するか
    void StartFadeOut(bool auto_destroy = false);

    //! @brief フェード完了か
    //! @retval true 完了
    //! @retval false 未完了
    bool IsFadeComplete() const
    {
        if(direction_ > 0)
        {
            return alpha_rate_ >= 1.0f;
        }
        else
        {
            return alpha_rate_ <= 0.0f;
        }
    }

private:
    float alpha_rate_  = 0.0f;    //!< 透明度
    float fade_time_   = 1.0f;    //!< フェードの時間
    bool is_start_     = false;   //!< フェード開始フラグ
    bool auto_destroy_ = false;   //!< 自動削除フラグ
    int direction_     = 1;       //!< フェード方向
};

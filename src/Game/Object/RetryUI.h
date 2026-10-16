#pragma once
//===========================================================================
//! @file RetryUI.h
//! @brief リトライUIコンポーネント
//===========================================================================

USING_PTR(RetryUI);

//---------------------------------------------------------------------------
//! @class RetryUI
//! @brief リトライUIコンポーネント
//---------------------------------------------------------------------------
class RetryUI : public Object
{
public:
    BP_OBJECT_DECL(RetryUI, u8"リトライUIコンポーネント");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 描画
    void PostDraw() override;

    //! @brief 終了
    void Exit() override;

private:
    int game_over_ghandle_ = -1;                  //!< ゲームオーバー画像ハンドル
    int retry_ghandle_     = -1;                  //!< リトライ画像ハンドル
    std::array<int, 2> retry_button_ghandle_{};   //!< リトライボタン画像ハンドル

    float alpha_time_ = 0.0f;   //!< アルファ値の時間

    static constexpr float SCENE_CHANGE_TIME_ = 2.0f;    //!< シーン切り替え時間
    float scene_change_time_                  = 0.0f;    //!< シーン切り替え経過時間
    float press_key_offset_                   = 0.0f;    //!< キー押下時のオフセット
    bool start_change_                        = false;   //!< 切り替え開始フラグ
};

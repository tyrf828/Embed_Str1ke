//---------------------------------------------------------------------------
//!	@file	Pause.h
//! @brief	ポーズ画面
//---------------------------------------------------------------------------
#pragma once

//===========================================================================
//!	@class GamePause
//!	@brief ポーズ画面クラス
//===========================================================================
class GamePause
{
public:
    //! @brief 初期化
    void Init();

    //! @brief 更新
    void Update();

    //! @brief 描画
    void Draw();

    //! @brief 終了
    void Exit();

    //! @brief ポーズ画面を開く
    //! @param is_set_pause ポーズ状態にするか
    void OpenPause(bool is_set_pause);

    //! @brief ウィンドウが閉じているか取得
    //! @return true:閉じている false:開いている
    bool GetWindow();

    //! @brief ウィンドウのスケールに応じたX座標を取得
    //! @param scale スケール
    //! @return X座標
    float WindowScaleX(float scale);

private:
    //----------------------------------------------------------
    //! @enum   Selected
    //! どこに選択しているか
    //----------------------------------------------------------
    enum SELECTED
    {
        MUSIC,    //!< BGM音量
        SE,       //!< SE音量
        RESUME,   //!< 再開
        EXIT,     //!< 終了

        MAX,
    };
    static constexpr int CURSOR_ALPHA_SPEED_ = 6;   //!< カーソルの点滅速度

    int select_color_[SELECTED::MAX] = {0};            //!< 選択肢の色
    float2 window_pos_               = {0.0f, 0.0f};   //!< ウィンドウの位置
    float2 window_size_              = {0.0f, 0.0f};   //!< ウィンドウのサイズ
    float2 music_pos_                = {0.0f, 0.0f};   //!< BGM音量バーの位置
    float2 se_pos_                   = {0.0f, 0.0f};   //!< SE音量バーの位置
    int select_mode_                 = 0;              //!< 現在の選択モード
    bool is_pause_                   = false;          //!< ポーズ中か
    bool is_closed_                  = false;          //!< ウィンドウが閉じているか
    float2 cursor_pos_               = {0.0f, 0.0f};   //!< カーソルの位置
    int cursor_alpha_                = 0;              //!< カーソルの透明度
    int cursor_alpha_mode_           = 0;              //!< カーソルの点滅モード

    int se_         = 0;   //!< SEハンドル
    int bar_se_     = 0;   //!< バー操作SEハンドル
    int confirm_se_ = 0;   //!< 決定SEハンドル
    int menu_se_    = 0;   //!< メニューSEハンドル

    //! @brief カーソルの透明度更新
    void CursorAlpha();
    //! @brief BGM選択更新
    void UpdateMusicSelect();
    //! @brief SE選択更新
    void UpdateSeSelect();
    //! @brief 再開選択更新
    void UpdateResumeSelect();
    //! @brief 終了選択更新
    void UpdateExitSelect();
    //! @brief ウィンドウアニメーション更新
    void UpdateWindowAnimation();

    using UpdateFunc = void (GamePause::*)();
    UpdateFunc update_func_[SELECTED::MAX];   //!< 更新関数テーブル
};

//!	@brief 初期化
bool PauseInit();

//!	@brief 更新
void PauseUpdate();

//!	@brief 描画
void PauseDraw();

//!	@brief 終了
void PauseExit();

//!	@brief ポーズを設定
void SetPause();

//!	@brief ポーズ画面を開閉する (true:開く / false:閉じる)
void PauseOpen(bool is_open);

//!	@brief ポーズ状態か取得
bool IsPause();

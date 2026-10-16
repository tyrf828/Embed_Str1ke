//---------------------------------------------------------------------------
//!	@file	Pause.cpp
//! @brief	ポーズ画面
//---------------------------------------------------------------------------
#include "Pause.h"
#include "WinMain.h"
#include <System/Lib.h>
#include <Game/Scene/SceneTitle.h>

static GamePause* pause = nullptr;

//---------------------------------------------------------------------------
//! @brief 初期化処理関数
//---------------------------------------------------------------------------
void GamePause::Init()
{
    // ウィンドウ位置・サイズの初期化
    window_pos_.x  = WINDOW_W * 0.2f;
    window_pos_.y  = WINDOW_H * 0.5f;
    window_size_.x = WINDOW_W - window_pos_.x * 2;
    window_size_.y = 0;

    // 音量バー位置の初期化
    music_pos_.x = WINDOW_W * 0.85f - (WINDOW_W * 0.85f - WINDOW_W * 0.45f) * MUSIC_VOLUME;
    music_pos_.y = WINDOW_H * 0.43f + GetFontSize() * 0.5f;
    se_pos_.x    = WINDOW_W * 0.85f - (WINDOW_W * 0.85f - WINDOW_W * 0.45f) * SE_VOLUME;
    se_pos_.y    = WINDOW_H * 0.5f + GetFontSize() * 0.5f;

    // カーソル初期化
    cursor_pos_.x      = WINDOW_W * 0.17f;
    cursor_pos_.y      = 0;
    cursor_alpha_      = 255;
    cursor_alpha_mode_ = 0;
    select_mode_       = 0;
    is_pause_          = false;
    is_closed_         = false;

    // SE読み込み
    se_         = LoadSoundMem("data/Game/SE/Pause/pause.mp3");
    bar_se_     = LoadSoundMem("data/Game/SE/Pause/se_volume.mp3");
    confirm_se_ = LoadSoundMem("data/Game/SE/Pause/confirm.mp3");
    menu_se_    = LoadSoundMem("data/Game/SE/Pause/menu.mp3");

    // 更新関数テーブル設定
    update_func_[SELECTED::MUSIC]  = &GamePause::UpdateMusicSelect;
    update_func_[SELECTED::SE]     = &GamePause::UpdateSeSelect;
    update_func_[SELECTED::RESUME] = &GamePause::UpdateResumeSelect;
    update_func_[SELECTED::EXIT]   = &GamePause::UpdateExitSelect;
}

//---------------------------------------------------------------------------
//! @brief 更新処理関数
//---------------------------------------------------------------------------
void GamePause::Update()
{
    // SE音量設定
    ChangeVolumeSoundMem((int)(255 * SE_VOLUME * 0.5f), se_);
    ChangeVolumeSoundMem((int)(255 * SE_VOLUME * 0.5f), bar_se_);
    ChangeVolumeSoundMem((int)(255 * SE_VOLUME * 0.5f), confirm_se_);
    ChangeVolumeSoundMem((int)(255 * SE_VOLUME * 0.5f), menu_se_);

    UpdateWindowAnimation();
    CursorAlpha();

    // 選択項目の更新
    if(select_mode_ >= 0 && select_mode_ < SELECTED::MAX)
    {
        (this->*update_func_[select_mode_])();
    }

    // 選択項目の色更新
    for(int i = 0; i < SELECTED::MAX; i++)
    {
        select_color_[i] = (select_mode_ == i) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);
    }

    // 音量バーの位置更新
    music_pos_.x = WindowScaleX(0.85f) - (WindowScaleX(0.85f) - WindowScaleX(0.45f)) * (1 - MUSIC_VOLUME);
    se_pos_.x    = WindowScaleX(0.85f) - (WindowScaleX(0.85f) - WindowScaleX(0.45f)) * (1 - SE_VOLUME);
}

//---------------------------------------------------------------------------
//! @brief 描画処理関数
//---------------------------------------------------------------------------
void GamePause::Draw()
{
    // ウィンドウ描画
    DrawBoxAA(window_pos_.x, window_pos_.y, window_pos_.x + window_size_.x, window_pos_.y + window_size_.y, GetColor(0, 0, 0), true);
    DrawBoxAA(window_pos_.x, window_pos_.y, window_pos_.x + window_size_.x, window_pos_.y + window_size_.y, GetColor(255, 255, 255), false);

    SetFontSize(DEFAULT_FONT_SIZE);

    int STR_HEIGHT              = GetFontSize();
    const float HALF_STR_HEIGHT = (float)STR_HEIGHT * 0.5f;

    // タイトル描画
    float window_h = WINDOW_H * 0.3f;
    if(window_pos_.y < window_h)
    {
        DrawStringMiddle(WindowScaleX(0.5f), window_h, "Setting", GetColor(255, 255, 255));
    }

    // BGM音量設定描画
    window_h = WINDOW_H * 0.43f;
    if(window_pos_.y < WINDOW_H * 0.43f)
    {
        DrawStringF(WindowScaleX(0.2f), window_h, "Music Volume", GetColor(255, 255, 255));
        DrawLineAA(WindowScaleX(0.45f), window_h + HALF_STR_HEIGHT, WindowScaleX(0.85f), WINDOW_H * 0.43f + HALF_STR_HEIGHT, GetColor(255, 255, 255));
        DrawCircleAA(music_pos_.x, music_pos_.y, 5, 64, select_color_[SELECTED::MUSIC], true);
    }

    // SE音量設定描画
    window_h = WINDOW_H * 0.5f;
    if(window_pos_.y < window_h)
    {
        DrawStringF(WindowScaleX(0.2f), window_h, "SE Volume", GetColor(255, 255, 255));
        DrawLineAA(WindowScaleX(0.45f), window_h + HALF_STR_HEIGHT, WindowScaleX(0.85f), WINDOW_H * 0.50f + HALF_STR_HEIGHT, GetColor(255, 255, 255));
        DrawCircleAA(se_pos_.x, se_pos_.y, 5, 64, select_color_[SELECTED::SE], true);
    }

    // 再開・終了ボタン描画
    window_h = WINDOW_H * 0.6f;
    if((float)(window_pos_.y + window_size_.y) > window_h)
    {
        DrawStringMiddle(WindowScaleX(0.35f), window_h + HALF_STR_HEIGHT, "Resume", select_color_[SELECTED::RESUME]);

        const char* exit_text = "Return Title";
        if(Scene::GetCurrentScene<SceneTitle>())
        {
            exit_text = "Exit";
        }
        DrawStringMiddle(WindowScaleX(0.65f), window_h + HALF_STR_HEIGHT, exit_text, select_color_[SELECTED::EXIT]);
    }

    // カーソル描画
    if(window_pos_.y < WINDOW_H * 0.3f)
    {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, cursor_alpha_);
        DrawTriangleAA(
            cursor_pos_.x, cursor_pos_.y, cursor_pos_.x - 20, cursor_pos_.y - 10, cursor_pos_.x - 20, cursor_pos_.y + 10, GetColor(255, 255, 255), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, -1);
    }

    SetFontSize(DEFAULT_FONT_SIZE);
}

//---------------------------------------------------------------------------
//! @brief 終了処理関数
//---------------------------------------------------------------------------
void GamePause::Exit()
{
    DeleteSoundMem(se_);
    DeleteSoundMem(bar_se_);
    DeleteSoundMem(confirm_se_);
    DeleteSoundMem(menu_se_);
}

//---------------------------------------------------------------------------
//! @brief ポーズを設定する
//---------------------------------------------------------------------------
void GamePause::OpenPause(bool is_set_pause)
{
    is_pause_  = is_set_pause;
    is_closed_ = false;
    if(is_set_pause)
    {
        select_mode_ = 0;
    }
    PlaySoundMem(menu_se_, DX_PLAYTYPE_BACK);
}

//---------------------------------------------------------------------------
//! @brief ポーズ画面閉じてるか
//---------------------------------------------------------------------------
bool GamePause::GetWindow()
{
    return is_closed_;
}

//---------------------------------------------------------------------------
//! @brief カーソルの透明度処理
//---------------------------------------------------------------------------
void GamePause::CursorAlpha()
{
    if(cursor_alpha_ >= 255)
    {
        cursor_alpha_mode_ = 0;
        cursor_alpha_      = 255;
    }
    if(cursor_alpha_ <= 0)
    {
        cursor_alpha_mode_ = 1;
        cursor_alpha_      = 0;
    }

    if(cursor_alpha_mode_ == 0)
    {
        cursor_alpha_ -= (int)(CURSOR_ALPHA_SPEED_ * GetDeltaTime60());
    }
    else
    {
        cursor_alpha_ += (int)(CURSOR_ALPHA_SPEED_ * GetDeltaTime60());
    }
}

//---------------------------------------------------------------------------
//! @brief ポーズの横画面のスケール座標を取る
//---------------------------------------------------------------------------
float GamePause::WindowScaleX(float scale)
{
    const float window_resoultion_x = (window_size_.x + window_pos_.x) - window_pos_.x;
    return (window_resoultion_x * scale) + window_pos_.x;
}

//---------------------------------------------------------------------------
//! @brief ウィンドウアニメーション更新
//---------------------------------------------------------------------------
void GamePause::UpdateWindowAnimation()
{
    is_closed_ = false;

    if(is_pause_)
    {
        // 開くアニメーション
        const float window_top_pos_y    = WINDOW_H * 0.2f;
        const float window_bottom_pos_y = (WINDOW_H * (WINDOW_H - window_top_pos_y) / WINDOW_H) - window_top_pos_y;
        if(window_pos_.y > window_top_pos_y)
        {
            window_pos_.y -= 10 * GetDeltaTime60();
        }
        else
        {
            window_pos_.y = window_top_pos_y;
        }
        if(window_size_.y < window_bottom_pos_y)
        {
            window_size_.y += 20 * GetDeltaTime60();
        }
        else
        {
            window_size_.y = window_bottom_pos_y;
        }
    }
    else
    {
        // 閉じるアニメーション
        if(window_pos_.y < WINDOW_H * 0.5f)
        {
            window_pos_.y += 10 * GetDeltaTime60();
        }
        else
        {
            window_pos_.y = WINDOW_H * 0.5f;
        }
        if(window_size_.y > 0.0f)
        {
            window_size_.y -= 20 * GetDeltaTime60();
        }
        else
        {
            window_size_.y = 0.0f;
        }

        if(window_pos_.y == WINDOW_H * 0.5f && window_size_.y == 0.0f)
        {
            is_closed_ = true;
        }
    }
}

//---------------------------------------------------------------------------
//! @brief MUSIC選択時の更新
//---------------------------------------------------------------------------
void GamePause::UpdateMusicSelect()
{
    cursor_pos_.x = WindowScaleX(0.17f);
    cursor_pos_.y = WINDOW_H * 0.43f + GetFontSize() * 0.5f;

    if(IsKeyOn(KEY_INPUT_DOWN) || IsPadOn(PAD_ID::PAD_D_DOWN))
    {
        select_mode_++;
        PlaySoundMem(se_, DX_PLAYTYPE_BACK);
    }

    if(IsKeyOn(KEY_INPUT_RIGHT) || IsPadOn(PAD_ID::PAD_D_RIGHT))
    {
        MUSIC_VOLUME += 0.1f;
    }
    if(IsKeyOn(KEY_INPUT_LEFT) || IsPadOn(PAD_ID::PAD_D_LEFT))
    {
        MUSIC_VOLUME -= 0.1f;
    }
    MUSIC_VOLUME = std::clamp(MUSIC_VOLUME, 0.0f, 1.0f);
}

//---------------------------------------------------------------------------
//! @brief SE選択時の更新
//---------------------------------------------------------------------------
void GamePause::UpdateSeSelect()
{
    cursor_pos_.x = WindowScaleX(0.17f);
    cursor_pos_.y = WINDOW_H * 0.50f + GetFontSize() * 0.5f;

    if(IsKeyOn(KEY_INPUT_UP) || IsPadOn(PAD_ID::PAD_D_UP))
    {
        select_mode_--;
        PlaySoundMem(se_, DX_PLAYTYPE_BACK);
    }
    if(IsKeyOn(KEY_INPUT_DOWN) || IsPadOn(PAD_ID::PAD_D_DOWN))
    {
        select_mode_++;
        PlaySoundMem(se_, DX_PLAYTYPE_BACK);
    }

    if(IsKeyOn(KEY_INPUT_RIGHT) || IsPadOn(PAD_ID::PAD_D_RIGHT))
    {
        SE_VOLUME += 0.1f;
        PlaySoundMem(bar_se_, DX_PLAYTYPE_BACK);
    }
    if(IsKeyOn(KEY_INPUT_LEFT) || IsPadOn(PAD_ID::PAD_D_LEFT))
    {
        SE_VOLUME -= 0.1f;
        PlaySoundMem(bar_se_, DX_PLAYTYPE_BACK);
    }
    SE_VOLUME = std::clamp(SE_VOLUME, 0.0f, 1.0f);
}

//---------------------------------------------------------------------------
//! @brief RESUME選択時の更新
//---------------------------------------------------------------------------
void GamePause::UpdateResumeSelect()
{
    cursor_pos_.x = WindowScaleX(0.28f);
    cursor_pos_.y = WINDOW_H * 0.6f + GetFontSize() * 0.5f;

    if(IsKeyOn(KEY_INPUT_UP) || IsKeyOn(KEY_INPUT_LEFT) || IsPadOn(PAD_ID::PAD_D_UP) || IsPadOn(PAD_ID::PAD_D_LEFT))
    {
        select_mode_--;
        PlaySoundMem(se_, DX_PLAYTYPE_BACK);
    }
    if(IsKeyOn(KEY_INPUT_DOWN) || IsKeyOn(KEY_INPUT_RIGHT) || IsPadOn(PAD_ID::PAD_D_DOWN) || IsPadOn(PAD_ID::PAD_D_RIGHT))
    {
        select_mode_++;
        PlaySoundMem(se_, DX_PLAYTYPE_BACK);
    }

    if(IsKeyOn(KEY_INPUT_RETURN) || IsKeyOn(KEY_INPUT_SPACE) || IsPadOn(PAD_ID::PAD_A))
    {
        is_pause_ = false;
        PlaySoundMem(confirm_se_, DX_PLAYTYPE_BACK);
    }
}

//---------------------------------------------------------------------------
//! @brief EXIT選択時の更新
//---------------------------------------------------------------------------
void GamePause::UpdateExitSelect()
{
    cursor_pos_.x = WindowScaleX(0.53f);
    cursor_pos_.y = WINDOW_H * 0.6f + GetFontSize() * 0.5f;

    if(IsKeyOn(KEY_INPUT_UP) || IsKeyOn(KEY_INPUT_LEFT) || IsPadOn(PAD_ID::PAD_D_UP) || IsPadOn(PAD_ID::PAD_D_LEFT))
    {
        select_mode_--;
        PlaySoundMem(se_, DX_PLAYTYPE_BACK);
    }

    if(IsKeyOn(KEY_INPUT_RETURN) || IsKeyOn(KEY_INPUT_SPACE) || IsPadOn(PAD_ID::PAD_A))
    {
        if(Scene::GetCurrentScene<SceneTitle>())
        {
            IS_EXIT = true;
            return;
        }

        IS_PAUSE = false;
        PlaySoundMem(confirm_se_, DX_PLAYTYPE_BACK);
        window_pos_.y  = WINDOW_H * 0.5f;
        window_size_.y = 0.0f;
        Scene::Change(Scene::GetScene<SceneTitle>());
    }
}

//---------------------------------------------------------------------------------
//!	初期化
//---------------------------------------------------------------------------------
bool PauseInit()
{
    if(!pause)
    {
        pause = new GamePause();
    }
    pause->Init();
    return true;
}

//---------------------------------------------------------------------------------
//!	更新
//---------------------------------------------------------------------------------
void PauseUpdate()
{
    if(pause)
    {
        if((IsKeyOn(KEY_INPUT_ESCAPE) || IsPadOn(PAD_ID::PAD_BACK)) && !IS_PAUSE)
        {
            // ゲームをポーズする
            IS_PAUSE = true;
            // ポーズ画面を開く
            pause->OpenPause(true);
        }
        else if((IsKeyOn(KEY_INPUT_ESCAPE) || IsPadOn(PAD_ID::PAD_BACK)) && IS_PAUSE)
        {
            // ポーズ画面を閉じる
            pause->OpenPause(false);
        }
        // ポーズ画面を完全に閉じたらゲームに戻る
        if(pause->GetWindow())
        {
            IS_PAUSE = false;
        }
        if(IS_PAUSE)
        {
            pause->Update();
        }
    }
}

//---------------------------------------------------------------------------------
//!	描画
//---------------------------------------------------------------------------------
void PauseDraw()
{
    if(pause && IS_PAUSE)
    {
        pause->Draw();
    }
}

//---------------------------------------------------------------------------------
//!	終了
//---------------------------------------------------------------------------------
void PauseExit()
{
    if(pause)
    {
        pause->Exit();
        delete pause;
        pause = nullptr;
    }
}

//---------------------------------------------------------------------------------
//!	ポーズを設定
//---------------------------------------------------------------------------------
void SetPause()
{
    if(pause)
    {
        pause->OpenPause(true);
    }
    IS_PAUSE = true;
}

//---------------------------------------------------------------------------------
//!	ポーズ画面を開閉する
//---------------------------------------------------------------------------------
void PauseOpen(bool is_open)
{
    if(pause)
    {
        pause->OpenPause(is_open);
    }
}

bool IsPause()
{
    return IS_PAUSE;
}

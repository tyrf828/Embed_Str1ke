//===========================================================================
//! @file RetryUI.cpp
//! @brief リトライUIコンポーネント
//===========================================================================
#include "RetryUI.h"
#include <Game/Scene/SceneTitle.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
//! @return 初期化済み
bool RetryUI::Init()
{
    Super::Init();
    // 画像読み込み
    game_over_ghandle_       = LoadGraph("data/Game/GHandle/gameover.png");
    retry_ghandle_           = LoadGraph("data/Game/GHandle/retry.png");
    retry_button_ghandle_[0] = LoadGraph("data/Game/GHandle/Keyboard & Mouse/Default/keyboard_space.png");
    retry_button_ghandle_[1] = LoadGraph("data/Game/GHandle/Xbox Series/Default/xbox_button_a.png");

    AddComponent<Sound>()->Load2D({
        {"decide", "data/Game/SE/ETC/決定ボタンを押す22.mp3", 0.0f, 1.0f},
    });

    return true;
}

//! @brief 更新
void RetryUI::Update()
{
    Super::Update();
    // フェードインアニメーション
    alpha_time_ += GetDeltaTime();
    alpha_time_  = std::min(alpha_time_, 1.0f);
    if(alpha_time_ < 1.0f)
    {
        return;
    }

    // 入力検知
    if(IsKeyOn(KEY_INPUT_SPACE) || IsPadOn(PAD_ID::PAD_A))
    {
        if(!start_change_)
        {
            start_change_      = true;
            scene_change_time_ = SCENE_CHANGE_TIME_;
            press_key_offset_  = WINDOW_W * 0.001f;   // ボタンを押した演出用のオフセット
        }
        if(auto se = GetComponent<Sound>())
        {
            se->PlayOnce2D("decide");
        }
    }

    // シーン切り替え処理
    if(!start_change_)
    {
        return;
    }
    scene_change_time_ -= GetDeltaTime();
    scene_change_time_  = std::max(scene_change_time_, 0.0f);

    // ボタンの押し込み演出を戻す
    if(scene_change_time_ <= SCENE_CHANGE_TIME_ - 0.1f)
    {
        press_key_offset_ = 0.0f;
    }

    // 時間経過でタイトルへ遷移
    if(scene_change_time_ <= 0.0f)
    {
        // シーン切り替え
        Scene::Change(Scene::GetScene<SceneTitle>());
    }
}

//! @brief 描画
void RetryUI::PostDraw()
{
    Super::PostDraw();

    // 背景を黒で塗りつぶし
    DrawFillBox(0, 0, WINDOW_W, WINDOW_H, GetColor(0, 0, 0));

    // ゲームオーバーロゴ描画
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255.0f * alpha_time_));
    DrawRotaGraphF(WINDOW_W * 0.5f, WINDOW_H * 0.2f, 1.0 * SCALE_WINDOW, 0.0, game_over_ghandle_, true);

    // リトライボタン描画
    const float RETRY_Y = WINDOW_H * 0.72f;
    DrawRotaGraphF(WINDOW_W * 0.5f, RETRY_Y, 0.35 * SCALE_WINDOW, 0.0, retry_ghandle_, true);
    DrawCircleAA(WINDOW_W * 0.5f, RETRY_Y, 80.0f * SCALE_WINDOW, 30, GetColor(255, 255, 255), false, 3.0f);

    // ボタン表示
    if(GetConnecetdPadNum() == -1)
    {
        DrawRotaGraphF(
            WINDOW_W * 0.5f + press_key_offset_, RETRY_Y + WINDOW_H * 0.13f + press_key_offset_, 1.3 * SCALE_WINDOW, 0.0, retry_button_ghandle_[0], true);
    }
    else
    {
        DrawRotaGraphF(
            WINDOW_W * 0.5f + press_key_offset_, RETRY_Y + WINDOW_H * 0.13f + press_key_offset_, 1.0 * SCALE_WINDOW, 0.0, retry_button_ghandle_[1], true);
    }

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, -1);
}

//! @brief 終了
void RetryUI::Exit()
{
    Super::Exit();
    // 画像リソースの解放
    DeleteGraph(game_over_ghandle_);
    DeleteGraph(retry_ghandle_);
    for(int i = 0; i < retry_button_ghandle_.size(); i++)
    {
        DeleteGraph(retry_button_ghandle_[i]);
    }
}

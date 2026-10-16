//===========================================================================
//! @file SceneTitleManager.cpp
//! @brief タイトルシーンマネージャー
//===========================================================================
#include "SceneTitleManager.h"
#include <Game/Object/Bridge.h>
#include <Game/Object/Truck.h>
#include <Game/Scene/SceneTutorial.h>
#include <System/Lib.h>
#include <Game/Component/LineTransition.h>
#include <Game/Object/Effect/Rose2D.h>
#include <Game/Component/Sound.h>
#include <Game/Pause.h>
#include <Game/Object/FilterBloom.h>

//! @brief 初期化
//! @return 初期化済み
bool SceneTitleManager::Init()
{
    Super::Init();

    // フォントハンドルの作成
    font_handle_        = CreateFontToHandle(NULL, 30, int(4 * SCALE_WINDOW), DX_FONTTYPE_ANTIALIASING_EDGE);
    select_font_handle_ = CreateFontToHandle(NULL, 22, int(4 * SCALE_WINDOW), DX_FONTTYPE_ANTIALIASING_EDGE);

    // 画像の読み込み
    title_ghandle_     = LoadGraph("data/Game/GHandle/title.png");
    text_back_ghandle_ = LoadGraph("data/Game/GHandle/Title/Gamebottom.png");

    select_button_ghandle_[0] = LoadGraph("data/Game/GHandle/Keyboard & Mouse/Default/keyboard_space.png");
    select_button_ghandle_[1] = LoadGraph("data/Game/GHandle/Xbox Series/Default/xbox_button_color_a.png");

    // 選択肢機能の初期化
    InitSelectFunctions();

    // サウンドの読み込み
    AddComponent<Sound>()->Load2D({
        {"change_select", "data/Game/SE/ETC/カーソル移動10.mp3",     0.0f, 0.4f},
        {"decide",        "data/Game/SE/ETC/決定ボタンを押す22.mp3", 0.0f, 0.5f},
    });

    return true;
}

//! @brief 更新
void SceneTitleManager::Update()
{
    Super::Update();

    auto se = GetComponent<Sound>();

    // 各オブジェクトの更新
    BridgeUpdate();
    CameraUpdate();
    PlayerUpdate();

    // スタートしていない場合の入力処理
    if(!is_start_)
    {
        // 上入力
        if(IsKeyOn(KEY_INPUT_UP) || IsKeyOn(KEY_INPUT_W) || IsPadOn(PAD_ID::PAD_D_UP))
        {
            if(now_select_ > 0)
            {
                now_select_--;
                if(se)
                {
                    se->Play2D("change_select");
                }
            }
        }
        // 下入力
        if(IsKeyOn(KEY_INPUT_DOWN) || IsKeyOn(KEY_INPUT_S) || IsPadOn(PAD_ID::PAD_D_DOWN))
        {
            if(now_select_ < SELECT_MAX_ - 1)
            {
                now_select_++;
                if(se)
                {
                    se->Play2D("change_select");
                }
            }
        }
        // 決定入力
        if(IsKeyOn(KEY_INPUT_SPACE) || IsKeyOn(KEY_INPUT_RETURN) || IsPadOn(PAD_ID::PAD_A))
        {
            ExecuteSelectFunction();
            if(se)
            {
                se->Play2D("decide");
            }
        }
    }

    // 各選択肢のアニメーション更新
    for(int i = 0; i < SELECT_MAX_; i++)
    {
        if(i == now_select_)
        {
            // 選択されている項目は右に移動
            select_offset_x_[i] += SELECT_MOVE_SPEED_ * GetDeltaTime60();
            select_offset_x_[i]  = std::min(select_offset_x_[i], SELECT_MAX_OFFSET_);
        }
        else
        {
            // 選択されていない項目は元の位置に戻る
            select_offset_x_[i] -= SELECT_MOVE_SPEED_ * GetDeltaTime60();
            select_offset_x_[i]  = std::max(select_offset_x_[i], 0.0f);
        }
    }

    // スタート演出
    if(is_start_)
    {
        VECTOR truck_2d = ConvWorldPosToScreenPos(cast(player_->GetTranslate()));
        if(!truck_speed_up_)
        {
            // トラックが画面中央に来るまで移動
            truck_->AddTranslate(float3{SCENE_MOVE_SPEED_ * 0.25f, 0.0f, 0.0f});
            if(truck_2d.x < WINDOW_W * 0.55f)
            {
                truck_speed_up_ = true;
                // トランジション開始
                if(line_transition_)
                {
                    line_transition_->StartTransition(1.0f);
                }
            }
            // 停止音再生
            if(auto truck_se = truck_->GetComponent<Sound>())
            {
                truck_se->PlayOnce("truck_stop");
            }
        }
        else
        {
            // トラック加速
            truck_->AddTranslate(float3{-SCENE_MOVE_SPEED_ * 0.5f, 0.0f, 0.0f});
            // トランジション完了でシーン遷移
            if(line_transition_ && line_transition_->IsFinishTransition())
            {
                Scene::Change(Scene::GetScene<SceneTutorial>());
            }
            // 発進音再生
            if(auto truck_se = truck_->GetComponent<Sound>())
            {
                truck_se->Stop("truck_stop");
                truck_se->PlayOnce("truck_start");
            }
        }
    }
}

//! @brief 遅延描画
void SceneTitleManager::LateDraw()
{
    Super::LateDraw();
    // タイトルロゴ描画
    int size_x, size_y;
    GetGraphSize(title_ghandle_, &size_x, &size_y);
    DrawRotaGraphF(WINDOW_W * 0.1f + size_x * 0.25f * SCALE_WINDOW, WINDOW_H * 0.2f, 0.5f * SCALE_WINDOW, 0.0f, title_ghandle_, TRUE);

    int text_size_x, text_size_y;
    GetGraphSize(text_back_ghandle_, &text_size_x, &text_size_y);

    // 選択肢描画
    for(int i = 0; i < SELECT_MAX_; i++)
    {
        float pos_x = WINDOW_W * 0.1f + text_size_x * 0.25f + select_offset_x_[i];
        float pos_y = WINDOW_H * 0.6f + i * WINDOW_H * 0.15f;

        // 背景描画
        DrawRotaGraph3F(pos_x, pos_y, text_size_x * 0.5f, text_size_y * 0.5f, 0.45f * SCALE_WINDOW, 0.35f * SCALE_WINDOW, 0.0f, text_back_ghandle_, TRUE);

        int color = GetColor(255, 255, 255);
#if DEBUG
        if(i == 1)

        {
            color = GetColor(64, 64, 64);
        }
#endif
        // テキスト描画
        DrawStringMiddleToHandle(pos_x, pos_y, select_text_[i].c_str(), color, font_handle_);
        // 選択中のエフェクト位置更新
        if(i == now_select_ && rose_effect_)
        {
            rose_effect_->SetDrawPos(float2{WINDOW_W * 0.1f + text_size_x * 0.5f + select_offset_x_[i], pos_y});
        }
    }
    // 操作説明描画
    DrawStringMiddleToHandle(WINDOW_W * 0.48f, WINDOW_H * 0.9f, "選択：", GetColor(255, 255, 255), select_font_handle_);

    DrawRotaGraphF(WINDOW_W * 0.53f, WINDOW_H * 0.9f, 1.1f * SCALE_WINDOW, 0.0f, select_button_ghandle_[0], TRUE);

    DrawRotaGraphF(WINDOW_W * 0.58f, WINDOW_H * 0.9f, 0.8f * SCALE_WINDOW, 0.0f, select_button_ghandle_[1], TRUE);
}

//! @brief 終了
void SceneTitleManager::Exit()
{
    Super::Exit();
    // 解放
    DeleteGraph(title_ghandle_);
    DeleteGraph(text_back_ghandle_);
    DeleteFontToHandle(font_handle_);
    DeleteFontToHandle(select_font_handle_);
    for(int i = 0; i < select_button_ghandle_.size(); i++)
    {
        DeleteGraph(select_button_ghandle_[i]);
    }
}

//! @brief 選択肢の機能を初期化
void SceneTitleManager::InitSelectFunctions()
{
    // スタート
    select_functions_[0] = [&]() {
        is_start_    = true;
        rose_effect_ = Scene::Object::Create<Rose2D>("Rose2D");
    };
#if NDEBUG
    // 設定
    select_functions_[1] = [&]() { SetPause(); };
#else
    select_functions_[1] = [&]() {};
#endif
    // 終了
    select_functions_[2] = [&]() { IS_EXIT = true; };
}

//! @brief 選択された機能を実行
void SceneTitleManager::ExecuteSelectFunction()
{
    if(select_functions_[now_select_])
    {
        select_functions_[now_select_]();
    }
}

//! @brief 橋の更新
void SceneTitleManager::BridgeUpdate()
{
    for(auto& bridge : bridge_)
    {
        if(bridge)
        {
            // 橋を移動させ、一定距離でループさせる
            bridge->AddTranslate(float3{SCENE_MOVE_SPEED_, 0.0f, 0.0f});
            if(bridge->GetTranslate().x >= 3100.0f * 0.5f)
            {
                float offset = bridge->GetTranslate().x - (3100.0f * 0.5f);
                bridge->SetTranslate(float3{-3100.0f * 1.5f + offset, bridge->GetTranslate().y, bridge->GetTranslate().z});
            }
        }
    }
}

//! @brief カメラの更新
void SceneTitleManager::CameraUpdate()
{
    if(camera_ && truck_)
    {
        // トラックの位置に合わせてカメラを移動
        float3 truck_pos = truck_->GetTranslate();
        auto camera_pos  = camera_->GetTranslate();
        camera_->SetTranslate(float3{camera_pos.x, truck_pos.y + CAMERA_OFFSET_Y_, camera_pos.z});
        // タイトルへ戻った後も、3Dサウンドの聞く位置をタイトルのカメラに合わせる
        Set3DSoundListenerPosAndFrontPos_UpVecY(cast(camera_->GetTranslate()), cast(-camera_->GetVectorAxisZ()));
    }
}

//! @brief プレイヤーの更新
void SceneTitleManager::PlayerUpdate()
{
    if(player_ && truck_)
    {
        // トラックの位置に合わせてプレイヤーを移動
        float3 truck_pos = truck_->GetTranslate();
        player_->SetTranslate(float3{truck_pos.x + 44.75f, player_->GetTranslate().y, truck_pos.z + 5.0f});
    }
}

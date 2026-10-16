//===========================================================================
//! @file GameUI.cpp
//! @brief ゲームのUI
//===========================================================================
#include "GameUI.h"
#include <Game/Object/Player/Player.h>
#include <Game/Component/Player/Stamina.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Component/Player/UltimateGauge.h>

//! @brief 初期化
//! @return 初期化済み
bool GameUI::Init()
{
    Super::Init();
    // 読み込み
    ultimate_button_image_[0] = LoadGraph("data/Game/GHandle/Keyboard & Mouse/Default/keyboard_v_outline.png");
    ultimate_button_image_[1] = LoadGraph("data/Game/GHandle/Keyboard & Mouse/Default/keyboard_v.png");
    ultimate_button_image_[2] = LoadGraph("data/Game/GHandle/Xbox Series/Default/xbox_button_color_y_outline.png");
    ultimate_button_image_[3] = LoadGraph("data/Game/GHandle/Xbox Series/Default/xbox_button_color_y.png");
    hp_bottle_image_          = LoadGraph("data/Game/GHandle/pipo-btleffect102d.png");
    return true;
}

//! @brief 更新
void GameUI::Update()
{
    Super::Update();
    // プレイヤーオブジェクトを取得
    auto player = Scene::Object::Get<Player>();
    if(player)
    {
        // プレイヤーのHPコンポーネントを取得
        if(auto player_hp = player->GetComponent<ComponentHP>())
        {
            // HPの割合を計算
            player_.hp_rate_ = (float)player_hp->GetHP() / (float)player_hp->GetMaxHP();
            player_.hp_rate_ = std::clamp(player_.hp_rate_, 0.0f, 1.0f);
            // HPアニメーションを更新
            BarAnimation(player_.anim_hp_rate_, player_.hp_rate_);
        }
        // プレイヤーのスタミナコンポーネントを取得
        if(auto player_sta = player->GetComponent<Stamina>())
        {
            // スタミナの割合を計算
            player_.staminia_rate_ = (float)player_sta->GetStamina() / (float)player_sta->GetMaxStamina();
            player_.staminia_rate_ = std::clamp(player_.staminia_rate_, 0.0f, 1.0f);
        }
        // プレイヤーの必殺技ゲージを取得
        if(auto player_ult = player->GetComponent<UltimateGauge>())
        {
            // ゲージの割合を計算
            player_.ultimate_rate_ = (float)player_ult->GetEnergy() / (float)player_ult->GetUltimateCost();
            player_.ultimate_rate_ = std::clamp(player_.ultimate_rate_, 0.0f, 1.0f);
        }
    }
}

//! @brief 描画
void GameUI::LateDraw()
{
    Super::LateDraw();

    if(!use_draw_ui_)
    {
        return;
    }

    // プレイヤーオブジェクトを取得
    auto player = Scene::Object::Get<Player>();
    if(player)
    {
        // プレイヤーバーを描画
        PlayerBarDraw();
        PlayerBottleDraw();

        // 円形ゲージ
        float2 center = {WINDOW_W * 0.38f, WINDOW_H * 0.1f};
        float radius  = WINDOW_H * 0.04f;
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
        DrawCircularGauge(center, radius, 1.0f, GetColor(192, 192, 192), 128, radius * 0.7f);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, -1);
        DrawCircularGauge(center, radius, player_.ultimate_rate_, GetColor(255, 255, 0), 128, radius * 0.7f);

        int img_index = 0;
        // 必殺技使用可能なら画像を変える
        if(player_.ultimate_rate_ >= 1.0f)
        {
            img_index = 1;
        }
        if(GetConnecetdPadNum() != -1)
        {
            img_index += 2;
        }
        DrawRotaGraphF(center.x, center.y, 0.5f * SCALE_WINDOW, 0.0f, ultimate_button_image_[img_index], true);
    }
}

//! @brief 終了
void GameUI::Exit()
{
    Super::Exit();
    for(int i = 0; i < ULTIMATE_IMAGE_; i++)
    {
        DeleteGraph(ultimate_button_image_[i]);
    }
    DeleteGraph(hp_bottle_image_);
}

//! @brief プレイヤーバーの長さを取得する
//! @return プレイヤーバーの長さ
float GameUI::GetPlayerBarLength() const
{
    return WINDOW_W * 0.05f + PLAYER_BAR_LENGTH_;
}

//! @brief プレイヤーバーの高さを取得する
//! @return プレイヤーバーの高さ
float GameUI::GetPlayerBarHeight() const
{
    return PLAYER_BAR_HEIGHT;
}

//! @brief UIを描画するか
void GameUI::UseDrawUI(bool use_draw_ui)
{
    use_draw_ui_ = use_draw_ui;
}

//! @brief バーのアニメーション処理
//! @param anim_rate アニメーションの割合
//! @param rate 実際の割合
void GameUI::BarAnimation(float& anim_rate, float& rate)
{
    // アニメーション中の割合が実際の割合より大きい場合
    if(anim_rate > rate)
    {
        anim_rate -= BAR_SPEED_ * GetDeltaTime60();
    }
    else
    {
        // 実際の割合に代入
        anim_rate = rate;
    }
}

//! @brief プレイヤーバーの描画処理
void GameUI::PlayerBarDraw()
{
    if(auto player = Scene::Object::Get<Player>())
    {
        const float2 HP_BAR_POS  = {WINDOW_W * 0.05f, WINDOW_H * 0.07f};
        const float2 HP_BAR_SIZE = {HP_BAR_POS.x, HP_BAR_POS.y + WINDOW_H * 0.02f};
        DrawBackgroundBar(HP_BAR_POS, HP_BAR_SIZE);
        // プレイヤーのHPアニメーションバーを描画
        DrawBoxAA(HP_BAR_POS.x, HP_BAR_POS.y, HP_BAR_SIZE.x + PLAYER_BAR_LENGTH_ * player_.anim_hp_rate_, HP_BAR_SIZE.y, GetColor(255, 127, 127), true);
        // プレイヤーの実際のHPバーを描画
        DrawBoxAA(HP_BAR_POS.x, HP_BAR_POS.y, HP_BAR_SIZE.x + PLAYER_BAR_LENGTH_ * player_.hp_rate_, HP_BAR_SIZE.y, GetColor(128, 255, 128), true);

        const float2 STAMIN_BAR_POS  = {WINDOW_W * 0.05f, HP_BAR_SIZE.y};
        const float2 STAMIN_BAR_SIZE = {STAMIN_BAR_POS.x, STAMIN_BAR_POS.y + WINDOW_H * 0.02f};
        DrawBackgroundBar(STAMIN_BAR_POS, STAMIN_BAR_SIZE);
        // プレイヤーのスタミナバーを描画
        DrawBoxAA(STAMIN_BAR_POS.x,
                  STAMIN_BAR_POS.y,
                  STAMIN_BAR_SIZE.x + PLAYER_BAR_LENGTH_ * player_.staminia_rate_,
                  STAMIN_BAR_SIZE.y,
                  GetColor(0, 255, 255),
                  true);
    }
}

//! @brief プレイヤーの回復ボトルの描画処理
void GameUI::PlayerBottleDraw()
{
    int size_x_buf, size_y_buf;
    GetGraphSize(hp_bottle_image_, &size_x_buf, &size_y_buf);
    if(auto player = Scene::Object::Get<Player>())
    {
        for(int i = 0; i < player->GetHPBottleAmount(); i++)
        {
            const float IMAGE_SCALE = 0.25f * SCALE_WINDOW;
            DrawRotaGraphF(WINDOW_W * 0.06f + i * (size_x_buf * IMAGE_SCALE), WINDOW_H * 0.15f, IMAGE_SCALE, 0.0f, hp_bottle_image_, true);
        }
    }
}

//! @brief 背景バーの描画処理
void GameUI::DrawBackgroundBar(float2 pos, float2 size)
{
    // 背景を描画
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
    DrawBoxAA(pos.x, pos.y, size.x, size.y, GetColor(128, 128, 128), true);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, -1);
}

//! @brief 円形ゲージの描画処理
void GameUI::DrawCircularGauge(float2 center, float radius, float rate, int color, int seg, float inner_radius)
{
    rate = std::clamp(rate, 0.0f, 1.0f);

    float full_angle = 2.0f * DX_PI_F;
    float draw_angle = full_angle * rate;
    float step       = full_angle / seg;

    for(int i = 0; i < seg; i++)
    {
        float a0 = step * i;
        float a1 = step * (i + 1);
        if(a0 >= draw_angle)
        {
            break;
        }

        float start = a0 - PI / 2.0f;
        float end   = a1 - PI / 2.0f;

        float2 point1 = {center.x + cosf(start) * radius, center.y + sinf(start) * radius};
        float2 point2 = {center.x + cosf(end) * radius, center.y + sinf(end) * radius};
        if(inner_radius <= 0.0f)
        {
            DrawTriangleAA(center.x, center.y, point1.x, point1.y, point2.x, point2.y, color, true);
        }
        else
        {
            float2 i0 = {center.x + cosf(start) * inner_radius, center.y + sinf(start) * inner_radius};
            float2 i1 = {center.x + cosf(end) * inner_radius, center.y + sinf(end) * inner_radius};
            DrawTriangleAA(i0.x, i0.y, point1.x, point1.y, point2.x, point2.y, color, true);
            DrawTriangleAA(i0.x, i0.y, point2.x, point2.y, i1.x, i1.y, color, true);
        }
    }
}

//===========================================================================
//! @file DamageText.cpp
//! @brief ゲーム内のダメージ表示
//===========================================================================
#include "DamageText.h"

int DamageText::font_handle_ = -1;   //!< フォントハンドル

//! @brief 初期化
bool DamageText::Init()
{
    Super::Init();

    // フォントハンドル作成
    if(font_handle_ >= 0)
    {
        return true;
    }
    font_handle_ = CreateFontToHandle(NULL, int(24 * SCALE_WINDOW), 2, DX_FONTTYPE_ANTIALIASING);
    return true;
}

//! @brief 更新
void DamageText::Update()
{
    Super::Update();

    // 生存時間更新
    elapsed_ += GetDeltaTime();
    if(elapsed_ >= LIFE_)
    {
        Scene::Object::Release(shared_from_this());
    }
}

//! @brief 描画
void DamageText::LateDraw()
{
    Super::LateDraw();

    float time_ratio = elapsed_ / LIFE_;
    time_ratio       = std::clamp(time_ratio, 0.0f, 1.0f);

    // 上昇
    float offset = RISE_PIXEL_ * time_ratio;

    // フェードアウト
    float alpha_ratio = 1.0f - time_ratio;
    int alpha         = (int)(alpha_ratio * 255);
    alpha             = std::clamp(alpha, 0, 255);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

    std::string str = std::to_string(damage_value_);

    // 影描画
    float2 shadow_pos = {screen_pos_.x + 2.0f, screen_pos_.y - offset + 2.0f};
    // 本体描画
    float2 draw_pos = {screen_pos_.x, screen_pos_.y - offset};

    DrawStringFToHandle((float)shadow_pos.x, (float)shadow_pos.y, str.c_str(), GetColor(0, 0, 0), font_handle_);
    DrawStringFToHandle((float)draw_pos.x, (float)draw_pos.y, str.c_str(), GetColor(255, 255, 255), font_handle_);

    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, -1);
}

//! @brief 終了
void DamageText::Exit()
{
    Super::Exit();
}

//! @brief ダメージ値とワールド座標を設定
void DamageText::Set(int damage, const float3& world_pos)
{
    damage_value_ = damage;
    // ワールド座標をスクリーン座標に変換
    VECTOR screen_pos = ConvWorldPosToScreenPos(cast(world_pos));
    screen_pos_       = {screen_pos.x, screen_pos.y};
}

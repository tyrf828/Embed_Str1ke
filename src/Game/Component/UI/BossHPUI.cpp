//===========================================================================
//! @file BossHPUI.cpp
//! @brief ボスのHP表示UIコンポーネント
//===========================================================================
#include "BossHPUI.h"
#include <Game/Object/Enemy/Boss/Boss.h>
#include <Game/Component/ComponentHP.h>

//! @brief 初期化
void BossHPUI::Init()
{
    Super::Init();
    StartOpenAnimation();
    boss_hp_rate_      = 1.0f;
    boss_anim_hp_rate_ = 1.0f;
}

//! @brief 更新
void BossHPUI::Update()
{
    Super::Update();

    // ボスオブジェクトを取得
    auto boss = Scene::Object::Get<Boss>();
    if(boss)
    {
        // ボスのHPコンポーネントを取得
        if(auto boss_hp = boss->GetComponent<ComponentHP>())
        {
            // HPの割合を計算
            boss_hp_rate_ = (float)boss_hp->GetHP() / (float)boss_hp->GetMaxHP();
            boss_hp_rate_ = std::clamp(boss_hp_rate_, 0.0f, 1.0f);

            // HPアニメーションを更新
            BarAnimation(boss_anim_hp_rate_, boss_hp_rate_);
        }
    }

    // ボスバー開く演出の更新
    if(boss_bar_opening_ && boss_bar_open_rate_ < 1.0f)
    {
        boss_bar_open_rate_ += BOSS_BAR_OPEN_SPEED_ * GetDeltaTime60();
        if(boss_bar_open_rate_ >= 1.0f)
        {
            boss_bar_open_rate_ = 1.0f;
        }
    }
}

//! @brief 描画
void BossHPUI::LateDraw()
{
    Super::LateDraw();

    auto boss = Scene::Object::Get<Boss>();
    if(!boss)
    {
        return;
    }

    // ボスバーの中心位置
    const float CENTER_X = WINDOW_W * 0.5f;
    const float CENTER_Y = BOSS_BAR_POS_Y_;

    // 開く演出の現在の幅
    const float CURRENT_HALF_LENGTH = (BOSS_BAR_MAX_LENGTH_ * 0.5f) * boss_bar_open_rate_;

    // 背景バー（灰色）を左右から描画
    if(boss_bar_open_rate_ > 0.0f)
    {
        // 左側の背景
        float2 bg_left_start = {CENTER_X - CURRENT_HALF_LENGTH, CENTER_Y};
        float2 bg_left_end   = {CENTER_X, CENTER_Y + BOSS_BAR_HEIGHT_};
        DrawBackgroundBar(bg_left_start, bg_left_end);

        // 右側の背景
        float2 bg_right_start = {CENTER_X, CENTER_Y};
        float2 bg_right_end   = {CENTER_X + CURRENT_HALF_LENGTH, CENTER_Y + BOSS_BAR_HEIGHT_};
        DrawBackgroundBar(bg_right_start, bg_right_end);

        // HPバーを描画
        if(boss_bar_open_rate_ >= 1.0f)
        {
            // アニメーションHP
            float anim_length    = BOSS_BAR_MAX_LENGTH_ * boss_anim_hp_rate_;
            float2 anim_hp_start = {CENTER_X - BOSS_BAR_MAX_LENGTH_ * 0.5f, CENTER_Y};
            float2 anim_hp_end   = {CENTER_X - BOSS_BAR_MAX_LENGTH_ * 0.5f + anim_length, CENTER_Y + BOSS_BAR_HEIGHT_};
            DrawBoxAA(anim_hp_start.x, anim_hp_start.y, anim_hp_end.x, anim_hp_end.y, GetColor(255, 127, 127), true);

            // 実際のHP
            float hp_length = BOSS_BAR_MAX_LENGTH_ * boss_hp_rate_;
            float2 hp_start = {CENTER_X - BOSS_BAR_MAX_LENGTH_ * 0.5f, CENTER_Y};
            float2 hp_end   = {CENTER_X - BOSS_BAR_MAX_LENGTH_ * 0.5f + hp_length, CENTER_Y + BOSS_BAR_HEIGHT_};

            // HPの割合に応じて色を変更
            int r = (int)(255 * (1.0f - boss_hp_rate_));
            int g = (int)(255 * boss_hp_rate_);
            DrawBoxAA(hp_start.x, hp_start.y, hp_end.x, hp_end.y, GetColor(r, g, 0), true);
        }

        // 枠線を描画
        DrawBoxAA(CENTER_X - CURRENT_HALF_LENGTH, CENTER_Y, CENTER_X + CURRENT_HALF_LENGTH, CENTER_Y + BOSS_BAR_HEIGHT_, GetColor(255, 255, 255), false);
    }
}

//! @brief GUI表示処理
void BossHPUI::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("BossHPUI"))
        {
            ImGui::Text(u8"ボスHP割合: %.2f", boss_hp_rate_);
            ImGui::Text(u8"アニメーションHP割合: %.2f", boss_anim_hp_rate_);
            if(ImGui::Button(u8"開く演出開始"))
            {
                StartOpenAnimation();
            }

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//! @brief ボスのHPバー開く演出を開始
void BossHPUI::StartOpenAnimation()
{
    boss_bar_opening_   = true;
    boss_bar_open_rate_ = 0.0f;
}

//! @brief ボスのHPバー開く演出が完了したか
bool BossHPUI::IsOpenAnimationComplete() const
{
    return boss_bar_open_rate_ >= 1.0f;
}

//! @brief バーのアニメーション処理
void BossHPUI::BarAnimation(float& anim_rate, float target_rate)
{
    // アニメーション中の割合が目標の割合より大きい場合
    if(anim_rate > target_rate)
    {
        anim_rate -= BAR_ANIM_SPEED_ * GetDeltaTime60();
        if(anim_rate < target_rate)
        {
            anim_rate = target_rate;
        }
    }
    else
    {
        // 実際の割合に代入
        anim_rate = target_rate;
    }
}

//! @brief 背景バーの描画
void BossHPUI::DrawBackgroundBar(float2 pos, float2 size)
{
    // 背景を描画
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
    DrawBoxAA(pos.x, pos.y, size.x, size.y, GetColor(64, 64, 64), true);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, -1);
}

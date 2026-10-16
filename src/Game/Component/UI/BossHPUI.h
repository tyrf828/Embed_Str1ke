#pragma once
//===========================================================================
//! @file BossHPUI.h
//! @brief ボスのHP表示UIコンポーネント
//===========================================================================

USING_PTR(BossHPUI);

//---------------------------------------------------------------------------
//! @class BossHPUI
//! @brief ボスのHP表示UIコンポーネント
//---------------------------------------------------------------------------
class BossHPUI : public Component
{
public:
    BP_COMPONENT_DECL(BossHPUI, u8"ボスHPUIコンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 描画
    void LateDraw() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief ボスのHPバー開く演出を開始
    void StartOpenAnimation();

    //! @brief ボスのHPバー開く演出が完了したか
    //! @return 完了したらtrue
    bool IsOpenAnimationComplete() const;

private:
    float boss_hp_rate_      = 1.0f;   //!< ボスのHP割合
    float boss_anim_hp_rate_ = 1.0f;   //!< ボスのHPアニメーション割合

    float boss_bar_open_rate_ = 0.0f;   //!< ボスバー開く演出の割合 (0.0 ~ 1.0)
    bool boss_bar_opening_    = true;   //!< ボスバーが開いているか

    const float BOSS_BAR_MAX_LENGTH_ = WINDOW_W * 0.60f;    //!< ボスバーの最大長さ
    const float BOSS_BAR_HEIGHT_     = WINDOW_H * 0.020f;   //!< ボスバーの高さ
    const float BOSS_BAR_POS_Y_      = WINDOW_H * 0.9f;     //!< ボスバーのY座標

    static constexpr float BOSS_BAR_OPEN_SPEED_ = 0.03f;    //!< ボスバー開くスピード
    static constexpr float BAR_ANIM_SPEED_      = 0.005f;   //!< HPバーのアニメーション速度

    //! @brief バーのアニメーション処理
    //! @param anim_rate アニメーションの割合
    //! @param target_rate 目標の割合
    void BarAnimation(float& anim_rate, float target_rate);

    //! @brief 背景バーの描画
    //! @param pos 開始位置
    //! @param size 終了位置
    void DrawBackgroundBar(float2 pos, float2 size);
};

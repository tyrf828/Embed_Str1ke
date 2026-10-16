#pragma once
//===========================================================================
//! @file GameUI.h
//! @brief ゲームのUI
//===========================================================================

USING_PTR(GameUI);
//----------------------------------------------------------
//! @class GameUI
//! @brief ゲームのUI
//----------------------------------------------------------
class GameUI : public Object
{
public:
    BP_OBJECT_DECL(GameUI, "GameUI");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 描画
    void LateDraw() override;

    //! @brief 終了
    void Exit() override;

    //! @brief プレイヤーバーの長さを取得する
    //! @return プレイヤーバーの長さ
    float GetPlayerBarLength() const;

    //! @brief プレイヤーバーの高さを取得する
    //! @return プレイヤーバーの高さ
    float GetPlayerBarHeight() const;

    //! @brief UIを描画するか
    //! @param use_draw_ui 描画するか
    void UseDrawUI(bool use_draw_ui = true);

private:
    //----------------------------------------------------------
    //! @struct Bar
    //! @brief HPとスタミナのバーを表す構造体
    //----------------------------------------------------------
    struct Bar
    {
        float hp_rate_       = 0.0f;   //!< HPの割合
        float anim_hp_rate_  = 0.0f;   //!< アニメーションのHP割合
        float staminia_rate_ = 0.0f;   //!< スタミナの割合
        float ultimate_rate_ = 0.0f;   //!< 必殺技の割合
    };

    Bar player_;                //!< プレイヤーのバー
    Bar boss_;                  //!< ボスのバー
    bool use_draw_ui_ = true;   //!< UIを描画するか

    const float PLAYER_BAR_LENGTH_ = WINDOW_W * 0.30f;    //!< プレイヤーバーの長さ
    const float PLAYER_BAR_HEIGHT  = WINDOW_H * 0.015f;   //!< プレイヤーバーの高さ

    static constexpr int ULTIMATE_IMAGE_        = 4;     //!< 必殺技のボタン画像の数
    int ultimate_button_image_[ULTIMATE_IMAGE_] = {0};   //!< 必殺技のボタン画像(0-1:キーボード 2-3:コントローラー)

    int hp_bottle_image_ = 0;   //!< HPボトル画像

    static constexpr float BAR_SPEED_ = 0.005f;   //!< バーのアニメーション速度

    //! @brief バーのアニメーション処理
    //! @param anim_rate アニメーションの割合
    //! @param rate 実際の割合
    void BarAnimation(float& anim_rate, float& rate);

    //! @brief プレイヤーバーの描画処理
    void PlayerBarDraw();

    //! @brief プレイヤーの回復ボトルの描画処理
    void PlayerBottleDraw();

    //! @brief 背景バーの描画処理
    void DrawBackgroundBar(float2 pos, float2 size);

    //! @brief 円形ゲージの描画処理
    void DrawCircularGauge(float2 center, float radius, float rate, int color, int seg = 64, float inner_radius = 0.0f);
};

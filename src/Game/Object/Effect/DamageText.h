#pragma once
//===========================================================================
//! @file DamageText.h
//! @brief ゲーム内のダメージ表示
//===========================================================================

USING_PTR(DamageText);

//---------------------------------------------------------------------------
//! @class DamageText
//! @brief 武器が敵に当たった時のダメージ表示
//---------------------------------------------------------------------------
class DamageText : public Object
{
public:
    BP_OBJECT_DECL(DamageText, u8"ダメージ表示");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 描画
    void LateDraw() override;

    //! @brief 終了
    void Exit() override;

    //! @brief ダメージ値とワールド座標を設定
    //! @param damage ダメージ値
    //! @param world_pos ワールド座標
    void Set(int damage, const float3& world_pos);

private:
    static int font_handle_;               //!< フォントハンドル
    int damage_value_            = 0;      //!< ダメージ値
    float elapsed_               = 0.0f;   //!< 経過時間
    static constexpr float LIFE_ = 1.0f;   //!< 表示時間

    float2 screen_pos_                 = {0.0f, 0.0f};   //!< スクリーン座標
    static constexpr float RISE_PIXEL_ = 40.0f;          //!< 上昇するピクセル量
};

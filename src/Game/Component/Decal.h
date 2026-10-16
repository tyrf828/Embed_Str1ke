//---------------------------------------------------------------------------
//! @file   Decal.h
//! @brief  デカールコンポーネント
//---------------------------------------------------------------------------
#pragma once

USING_PTR(Decal);

//===========================================================================
//! デカール
//===========================================================================
class Decal : public Component
{
public:
    BP_COMPONENT_DECL(Decal, u8"デカール");

    //! @brief コンストラクタ
    //! @param owner オーナーオブジェクト
    void Construct(ObjectPtr owner)
    {
        __super::Construct(owner);
    }

    //! @brief コンストラクタ
    //! @param owner オーナーオブジェクト
    //! @param path テクスチャパス
    void Construct(ObjectPtr owner, std::string_view path)
    {
        __super::Construct(owner);

        SetDecalTexture(path);
    }

    //! @brief 初期化
    void Init() override;

    //! @brief 描画
    void Draw() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示
    void GUI() override;

    //! @brief デカールテクスチャの設定
    //! @param path パス
    void SetDecalTexture(std::string_view path)
    {
        if(!path.empty())
        {
            decal_texture_ = std::make_shared<Texture>(path);
        }
    }

    //! @brief 透明度の設定
    //! @param alpha 透明度
    void SetAlpha(float alpha)
    {
        alpha_ = std::clamp(alpha, 0.0f, 1.0f);
    }

private:
    struct DecalInfo
    {
        matrix mat_inverse_world_;   //!< ワールド逆行列
        float alpha_;                //!< 透明度
        float padding_[3];           //!< パディング
    };

    std::shared_ptr<ShaderPs> shader_ps_;      // 描画用テクスチャ
    std::shared_ptr<Texture> decal_texture_;   // ピクセルシェーダー
    int decal_cb_       = -1;                  // 定数バッファ
    bool draw_debug_box = false;               // デバッグ用のボックス

    float alpha_ = 1.0f;   // デカールの透明度
};

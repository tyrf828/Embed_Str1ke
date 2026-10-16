#pragma once
//===========================================================================
//! @file OutLine.h
//! @brief 輪郭線描画コンポーネント
//===========================================================================
#include <System/Component/Component.h>
#include <System/Component/ComponentModel.h>
#include <System/Graphics/Shader.h>

USING_PTR(OutLine);

//---------------------------------------------------------------------------
//! @class OutLine
//! @brief 輪郭線描画コンポーネントクラス
//---------------------------------------------------------------------------
class OutLine : public Component
{
public:
    BP_COMPONENT_DECL(OutLine, u8"輪郭線描画");

    //! @brief 初期化
    void Init() override;

    //! @brief スキャンによる強調表示の残り時間を更新する
    void Update() override;

    //! @brief スキャン表示
    //! @param seconds 表示時間
    void SetScanTime(float seconds)
    {
        scan_seconds_ = seconds;
    }

    //! @brief スキャン表示の時間を取得
    // @return 残り時間
    float GetScanTime() const
    {
        return scan_seconds_;
    }

    //! @brief 描画
    void Draw() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 輪郭線の太さを設定
    //! @param width 輪郭線の太さ
    void SetOutlineWidth(float width)
    {
        outline_width_ = width;
    }

    //! @brief 輪郭線の色を設定
    //! @param color 輪郭線の色
    void SetOutlineColor(const float4& color)
    {
        outline_color_ = color;
    }

    //! @brief 輪郭線の色を取得
    float4 GetOutlineColor() const
    {
        return outline_color_;
    }

    //! @brief 輪郭線描画の有効かを設定
    //! @param enabled 有効かどうか
    void SetEnabled(bool enabled)
    {
        enabled_ = enabled;
    }

    //! @brief 輪郭線描画が有効かどうかを取得
    //! @return 有効かどうか
    bool IsEnabled() const
    {
        return enabled_;
    }

private:
    //! @brief 輪郭線描画
    void DrawOutline();

    struct OutlineWidth
    {
        float outline_width_;   //!< 太さ
        float pos_[3];          //!< 座標
    };

    struct OutlineColor
    {
        float4 outline_color_;   //!< 色
    };

    std::shared_ptr<ShaderVs> shader_vs_ = nullptr;   //!< 頂点シェーダー
    std::shared_ptr<ShaderPs> shader_ps_ = nullptr;   //!< ピクセルシェーダー

    float outline_width_  = 0.01f;                      //!< 輪郭線の太さ
    float4 outline_color_ = {0.0f, 1.0f, 0.0f, 1.0f};   //!< 輪郭線の色
    bool enabled_         = true;                       //!< 輪郭線描画の有効/無効

    float scan_seconds_ = 0.0f;   //!< スキャン表示の残り時間

    int outline_cb_ = -1;   //!< 定数バッファ
    int color_cb_   = -1;   //!< 定数バッファ
};

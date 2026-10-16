#pragma once
//===========================================================================
//! @file   DissolveEffect.h
//! @brief  Dissolve エフェクトコンポーネント
//===========================================================================
#include <System/Component/Component.h>
#include <System/Graphics/Shader.h>
#include <System/Graphics/Texture.h>

USING_PTR(DissolveEffect);

class DissolveEffect : public Component
{
public:
    BP_COMPONENT_DECL(DissolveEffect, u8"Dissolve Effect");

    //! 初期化
    virtual void Init() override;

    //! 更新
    virtual void Update() override;

    //! 更新後処理
    virtual void Draw() override;

    //! 終了
    virtual void Exit() override;

    //! @brief GUI描画
    virtual void GUI() override;

    //! @brief ディゾルブ開始
    //! @param auto_destroy 完了時に自動削除するか
    void StartDissolve(bool auto_destroy = false)
    {
        dissolve_amount_ = -0.1f;
        auto_destroy_    = auto_destroy;
        reverse_         = false;
    }

    //! @brief 逆ディゾルブ開始
    //! @param auto_destroy 完了時に自動削除するか
    void StartReverseDissolve(bool auto_destroy = false)
    {
        dissolve_amount_ = 1.0f;
        auto_destroy_    = auto_destroy;
        reverse_         = true;
    }

    //! @brief ディゾルブ量を取得
    //! @return ディゾルブ量
    float GetDissolveAmount() const
    {
        return dissolve_amount_;
    }

    //! @brief アニメーション速度を設定
    //! @param speed 速度
    void SetAnimationSpeed(float speed)
    {
        animation_speed_ = speed;
    }

private:
    std::shared_ptr<ShaderPs> shader_ps_;      //!< ピクセルシェーダー
    std::shared_ptr<Texture> noise_texture_;   //!< ノイズテクスチャ
    struct DissolveInfo
    {
        float dissolve_amount_;   //!< ディゾルブ量
        float padding_[3];        //!< パディング
    };

    int dissolve_info_cb_  = -1;      //!< 定数バッファ
    float dissolve_amount_ = -0.1f;   //!< ディゾルブ量
    float animation_speed_ = 0.5f;    //!< アニメーション速度
    bool auto_destroy_     = false;   //!< 自動削除フラグ
    bool reverse_          = false;   //!< 逆再生フラグ
};

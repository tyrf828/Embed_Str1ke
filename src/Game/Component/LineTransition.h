#pragma once
//===========================================================================
//! @file   LineTransition.h
//! @brief  シーン転移(ラインエフェクト)
//===========================================================================
#include <System/Component/Component.h>
#include <System/Graphics/Shader.h>
#include <System/Graphics/Texture.h>

USING_PTR(LineTransition);

class LineTransition : public Component
{
public:
    BP_COMPONENT_DECL(LineTransition, u8"LineTransition");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI描画
    void GUI() override;

    //! @brief トランジション開始
    //! @param duration 完了までの時間
    void StartTransition(float duration = 1.0f)
    {
        if(duration <= 0.0f)
        {
            duration = 0.0001f;
        }
        transition_speed_  = 1.0f / duration;
        start_transition_  = true;
        finish_transition_ = false;
    }

    //! @brief トランジション進行度を取得
    //! @return 進行度 (0.0 ~ 1.0)
    float GetProgress() const
    {
        return transition_progress_;
    }

    //! @brief トランジション完了したかどうか
    //! @retval true 完了
    //! @retval false 未完了
    bool IsFinishTransition() const
    {
        return finish_transition_;
    }

    //! @brief トランジションをリセット
    void ResetTransition()
    {
        transition_progress_ = 0.0f;
        blend_ratio_         = 0.5f;
        start_transition_    = false;
        finish_transition_   = false;
    }

private:
    std::shared_ptr<ShaderPs> shader_ps_;   //!< ピクセルシェーダー
    int parameter_cb_ = -1;                 //!< 定数バッファ
    std::shared_ptr<Texture> texture_;      //!< テクスチャ

    struct TransitionInfo
    {
        float transition_progress_;   // トランジション
        float blend_ratio_;           // ブレンド率
        float padding_[2];            // パディング
    };

    float transition_progress_ = 0.0f;    //!< トランジション進行度
    float transition_speed_    = 1.0f;    //!< トランジション速度
    float blend_ratio_         = 0.5f;    //!< ブレンド率
    bool start_transition_     = false;   //!< トランジション開始フラグ
    bool finish_transition_    = false;   //!< トランジション完了フラグ
};

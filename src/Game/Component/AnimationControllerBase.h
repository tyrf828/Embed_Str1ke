#pragma once
//===========================================================================
//! @file AnimationControllerBase.h
//! @brief アニメーション制御の基底コンポーネント
//===========================================================================
#include <System/Component/Component.h>

//---------------------------------------------------------------------------
//! @class AnimationControllerBase
//! @brief アニメーション制御の基底コンポーネント
//---------------------------------------------------------------------------
class AnimationControllerBase : public Component
{
public:
    BP_COMPONENT_DECL(AnimationControllerBase, u8"アニメーション制御の基底コンポーネント");

    //! @brief 初期化
    void Init() override
    {
        Super::Init();
    }

    //! @brief 更新
    void Update() override
    {
        Super::Update();

        // 最後に立っているフラグを状態として設定
        for(int i = 0; i < anim_flag_max_; i++)
        {
            if(anim_flag_ & (1 << i))
            {
                anim_state_ = i;
            }
        }
    }

    //! @brief アニメーションフラグを設定
    //! @param anim_flag_bit アニメーションフラグ
    void SetFlag(int anim_flag_bit);

    //! @brief アニメーションフラグを削除
    //! @param anim_flag_bit アニメーションフラグ
    void ResetFlag(int anim_flag_bit);

    //! @brief アニメーションフラグの状態を取得
    //! @param anim_flag_bit 取得したいアニメーションフラグ
    //! @retval true フラグが立っている
    //! @retval false フラグが立っていない
    bool GetFlag(int anim_flag_bit) const;

    //! @brief 指定したアニメーションが今再生中のアニメーションなのか
    //! @param anim_flag アニメーションフラグ
    //! @retval true 再生中
    //! @retval false 再生されていない
    bool GetNowPlaying(int anim_flag) const;

    //! @brief アニメーション時間が指定範囲内か
    //! @param start_time 開始時間
    //! @param end_time 終了時間
    //! @retval true 範囲内
    //! @retval false 範囲外
    bool IsAnimationTimeInRange(float start_time, float end_time = D3D11_FLOAT32_MAX) const;

    //! @brief アニメーション再生中か
    //! @retval true 再生中
    //! @retval false 停止中
    bool IsPlaying() const;

    //! @brief 全てのフラグをリセット
    void ResetAllFlags();

    //! @brief タイムスケールを設定
    //! @param scale スケール
    void SetTimeScale(float scale);

protected:
    int anim_flag_     = 0;   //!< アニメーションフラグ
    int anim_state_    = 0;   //!< アニメーション状態
    int anim_flag_max_ = 0;   //!< アニメーションフラグ最大値

    //! @brief アニメーションが再生終了した際にリセット
    //! @param anim_flag_bit リセットするアニメーションフラグ
    void CheckPlayingResetFlag(int anim_flag_bit);
};

//===========================================================================
//! @file AnimationControllerBase.cpp
//! @brief アニメーション制御の基底コンポーネント
//===========================================================================
#include "AnimationControllerBase.h"
#include <System/Component/ComponentModel.h>

//! @brief アニメーションフラグを設定
//! @param anim_flag_num アニメーションフラグ
void AnimationControllerBase::SetFlag(int anim_flag_num)
{
    anim_flag_ |= anim_flag_num;
}

//! @brief アニメーションフラグを削除
//! @param anim_flag_bit アニメーションフラグ
void AnimationControllerBase::ResetFlag(int anim_flag_bit)
{
    anim_flag_ &= ~anim_flag_bit;
}

//! @brief アニメーションフラグの状態を取得
//! @param anim_flag_bit 取得したいアニメーションフラグ
//! @retval true フラグが立っている
//! @retval false フラグが立っていない
bool AnimationControllerBase::GetFlag(int anim_flag_bit) const
{
    return anim_flag_ & anim_flag_bit;
}

//! @brief 指定したアニメーションが今再生中のアニメーションなのか
//! @param anim_flag アニメーションフラグ
//! @retval true 再生中
//! @retval false 再生されていない
bool AnimationControllerBase::GetNowPlaying(int anim_flag) const
{
    return anim_state_ == anim_flag;
}

//! @brief アニメーション時間が指定範囲内か
//! @param start_time 開始時間
//! @param end_time 終了時間
//! @retval true 範囲内
//! @retval false 範囲外
bool AnimationControllerBase::IsAnimationTimeInRange(float start_time, float end_time) const
{
    if(auto model = GetOwner()->GetComponent<ComponentModel>())
    {
        float anim_time = model->GetAnimationTime() * model->GetAnimationSpeed();
        if(anim_time >= start_time && anim_time <= end_time)
        {
            return true;
        }
    }
    return false;
}

//! @brief アニメーション再生中か
//! @retval true 再生中
//! @retval false 停止中
bool AnimationControllerBase::IsPlaying() const
{
    if(auto model = GetOwner()->GetComponent<ComponentModel>())
    {
        return model->IsPlaying();
    }
    return false;
}

//! @brief 全てのフラグをリセット
void AnimationControllerBase::ResetAllFlags()
{
    for(int i = 0; i < anim_flag_max_; i++)
    {
        ResetFlag(1 << i);
    }
}

//! @brief タイムスケールを設定
//! @param scale スケール
void AnimationControllerBase::SetTimeScale(float scale)
{
    if(auto model = GetOwner()->GetComponent<ComponentModel>())
    {
        model->SetTimeScale(scale);
    }
}

//! @brief アニメーションが再生終了した際にリセット
//! @param anim_flag_bit リセットするアニメーションフラグ
void AnimationControllerBase::CheckPlayingResetFlag(int anim_flag_bit)
{
    if(auto model = GetOwner()->GetComponent<ComponentModel>())
    {
        // アニメーションが再生終了した際
        if(!model->IsPlaying())
        {
            // フラグを消す
            ResetFlag(anim_flag_bit);
        }
    }
}

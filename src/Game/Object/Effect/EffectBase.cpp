//===========================================================================
//! @file EffectBase.cpp
//! @brief エフェクトの基底クラス
//===========================================================================
#include "EffectBase.h"
#include <System/Component/ComponentEffect.h>

//! @brief エフェクト再生
//! @param loop ループするか
void EffectBase::Play(bool loop)
{
    auto effect = GetComponent<ComponentEffect>();
    if(effect)
    {
        // エフェクト再生
        effect->Play(loop);
        play_trigger_ = true;
    }
}

//! @brief エフェクトを終了させる
void EffectBase::Stop()
{
    auto effect = GetComponent<ComponentEffect>();
    if(effect)
    {
        // 終了させる
        effect->Stop();
    }
}

void EffectBase::Destroy(float second)
{
    destroy_timer_ = second;
    use_destroy_   = true;
}

//! @brief 遅い再生処理
void EffectBase::LatePlay()
{
    auto effect = GetComponent<ComponentEffect>();
    if(!effect)
    {
        return;
    }
    // トリガーがオフの場合
    if(!play_trigger_)
    {
        // エフェクトを再生
        effect->Play(false);
        // 再生トリガーを立つ
        play_trigger_ = true;
    }
}

//! @brief 自動破棄処理
//! @detail play_trigger_を使わない場合はfalseにしてください
//! @param use_trigger トリガーを使うか
void EffectBase::AutoDestroy(bool use_trigger)
{
    auto effect = GetComponent<ComponentEffect>();
    if(!effect)
    {
        return;
    }
    // トリガーを使わない場合は強制的に立てる
    if(!use_trigger)
    {
        play_trigger_ = true;
    }

    // エフェクトの再生が終了場合
    if(!effect->IsPlaying() && play_trigger_)
    {
        // 解放
        Scene::Object::Release(shared_from_this());
    }
}

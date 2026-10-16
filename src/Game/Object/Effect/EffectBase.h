#pragma once
//===========================================================================
//! @file EffectBase.h
//! @brief エフェクトの基底クラス
//===========================================================================

USING_PTR(EffectBase);

//---------------------------------------------------------------------------
//! @class EffectBase
//! @brief エフェクトの基底クラス
//---------------------------------------------------------------------------
class EffectBase : public Object
{
public:
    BP_OBJECT_DECL(EffectBase, "EffectBase");

    //! @brief 初期化
    //! @return 初期化済み
    virtual bool Init()
    {
        Super::Init();
        return true;
    }

    //! @brief 更新
    virtual void Update()
    {
        Super::Update();
        if(destroy_timer_ > 0.0f)
        {
            play_trigger_   = true;
            destroy_timer_ -= GetDeltaTime();
            if(destroy_timer_ <= 0.0f && use_destroy_)
            {
                Scene::Object::Release(shared_from_this());
            }
        }
    }

    //! @brief エフェクト再生
    //! @param loop ループするか
    virtual void Play(bool loop = false);

    //! @brief エフェクトを終了させる
    void Stop();

    //! @brief 何秒後に破棄するか
    //! @param second 破棄までの秒数
    void Destroy(float second);

protected:
    //---------------------------------------------------------------------------
    // Effekseerと連携する変数と関数
    //---------------------------------------------------------------------------
    bool play_trigger_ = false;   //!< エフェクトの再生トリガー

    //! @brief 遅い再生処理
    void LatePlay();

    //! @brief 自動破棄処理
    //! @detail play_trigger_を使わない場合はfalseにしてください
    //! @param use_trigger トリガーを使うか
    void AutoDestroy(bool use_trigger);

    //---------------------------------------------------------------------------
    // 一般的なエフェクトの変数と関数
    //---------------------------------------------------------------------------
    float destroy_timer_ = 0.0f;    //!< 破棄タイマー
    bool use_destroy_    = false;   //!< 破棄タイマーを使うか
};

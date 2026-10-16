#pragma once
//===========================================================================
//! @file ForceMoveBase.h
//! @brief 強制移動基底クラス
//===========================================================================
#include <System/Component/Component.h>
#include <System/Scene.h>

//---------------------------------------------------------------------------
//! @class ForceMoveBase
//! @brief 強制移動基底クラス
//---------------------------------------------------------------------------
class ForceMoveBase : public Component
{
public:
    BP_COMPONENT_DECL(ForceMoveBase, u8"強制移動基底クラス");

    //! @brief 方向を設定
    //! @param dir 方向
    //! @param force_power 力の強さ
    virtual void SetDirection(float3 dir, float force_power)
    {
        dir_         = normalize(dir);
        force_power_ = force_power;
    }

    //! @brief 初期化
    void Init() override
    {
        Super::Init();
    }

    //! @brief 更新
    void Update() override
    {
        Super::Update();

        // 移動
        GetOwner()->AddTranslate(float3{dir_ * force_power_});

        // 減衰
        damping_timer_ += GetDeltaTime();
        if(damping_timer_ >= 1.0f)
        {
            force_power_   *= damping_ratio_;
            damping_timer_  = 0.0f;
        }
    }

protected:
    float3 dir_          = {0.0f, 0.0f, 0.0f};   //!< 方向
    float force_power_   = 0.0f;                 //!< 力の強さ
    float damping_ratio_ = 0.9f;                 //!< 減衰率
    float damping_timer_ = 0.0f;                 //!< 減衰タイマー
};

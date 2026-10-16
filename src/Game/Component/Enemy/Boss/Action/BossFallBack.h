#pragma once
//===========================================================================
//! @file BossFallBack.h
//! @brief ボスの後退コンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class BossFallBack
//! @brief ボスの後退コンポーネント
//---------------------------------------------------------------------------
class BossFallBack : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(BossFallBack, u8"ボスの後退コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 速度を設定
    //! @param speed 速度
    void SetSpeed(float speed)
    {
        speed_ = speed;
    }

private:
    float prev_speed_ = 0.0f;   //!< 前のアクションの速度
    float speed_      = 2.0f;   //!< 後退速度
};

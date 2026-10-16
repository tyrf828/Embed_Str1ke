#pragma once
//===========================================================================
//! @file Attack3.h
//! @brief プレイヤーの攻撃コンポーネント
//===========================================================================
#include <Game/Component/Player/AttackBase.h>

//---------------------------------------------------------------------------
//! @class Attack3
//! @brief 攻撃コンポーネント
//---------------------------------------------------------------------------
class Attack3 : public AttackBase
{
public:
    BP_COMPONENT_DECL(Attack3, u8"攻撃コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 終了
    void Exit() override;

private:
    static constexpr float END_TIME_          = 2.65f;                      //!< アニメションの終了時間
    static constexpr float ATTACK_START_TIME_ = 1.82f;                      //!< 攻撃判定付与時間
    static constexpr float ATTACK_LIMIT_TIME_ = 2.18f;                      //!< 攻撃判定消す時間
    static constexpr int ATTACK_POINT_        = 150;                        //!< 攻撃力
    static constexpr int NEXT_ATTACK_STAMINA_ = 200;                        //!< 次の攻撃のスタミナの消耗量攻撃
    float3 input_dir_                         = float3(0.0f, 0.0f, 0.0f);   //!< 入力方向
};

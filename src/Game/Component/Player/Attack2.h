#pragma once
//===========================================================================
//! @file Attack2.h
//! @brief プレイヤーの攻撃コンポーネント
//===========================================================================
#include <Game/Component/Player/AttackBase.h>

//---------------------------------------------------------------------------
//! @class Attack2
//! @brief 攻撃コンポーネント
//---------------------------------------------------------------------------
class Attack2 : public AttackBase
{
public:
    BP_COMPONENT_DECL(Attack2, u8"攻撃コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 終了
    void Exit() override;

private:
    static constexpr float COMBO_START_TIME_  = 0.85f;                      //!< 次の攻撃に移行時間
    static constexpr float COMBO_LIMIT_TIME_  = 1.55f;                      //!< コンボ攻撃の制限時間
    static constexpr float ATTACK_START_TIME_ = 0.64f;                      //!< 攻撃判定付与時間
    static constexpr float ATTACK_LIMIT_TIME_ = 0.88f;                      //!< 攻撃判定消す時間
    static constexpr float END_TIME_          = 1.82f;                      //!< アニメションの終了時間
    static constexpr int NEXT_ATTACK_STAMINA_ = 120;                        //!< 次の攻撃のスタミナの消耗量攻撃
    static constexpr int ATTACK_POINT_        = 120;                        //!< 攻撃力
    float3 input_dir_                         = float3(0.0f, 0.0f, 0.0f);   //!< 入力方向
};

#pragma once
//===========================================================================
//! @file CombineAttack.h
//! @brief プレイヤーの武器組み立て攻撃
//===========================================================================
#include <Game/Component/Player/AttackBase.h>

//---------------------------------------------------------------------------
//! @class CombineAttack
//! @brief 武器組み立て攻撃
//---------------------------------------------------------------------------
class CombineAttack : public Component
{
public:
    BP_COMPONENT_DECL(CombineAttack, u8"武器組み立て攻撃");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 終了
    void Exit() override;

private:
    static constexpr float END_TIME_          = 3.22f;   //!< 攻撃アニメションの終了時間
    static constexpr float ATTACK_START_TIME_ = 0.0f;    //!< 攻撃判定付与時間

    //! @brief 攻撃判定を付ける
    void SetAttach();
};

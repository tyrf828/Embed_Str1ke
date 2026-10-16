//===========================================================================
//! @file UltimateBase.cpp
//! @brief 必殺技ベースクラス
//===========================================================================
#include "UltimateBase.h"

//! @brief オーナーを設定
//! @param owner オーナーオブジェクト
void UltimateBase::SetOwner(ObjectPtr owner)
{
    owner_ = owner;
}

//! @brief オーナーを取得
//! @return オーナーオブジェクト
ObjectPtr UltimateBase::GetOwner() const
{
    return owner_;
}

//! @brief 親コンポーネントを設定
//! @param ultimate_attack 親コンポーネント
void UltimateBase::SetParent(UltimateAttack* ultimate_attack)
{
    ultimate_attack_ = ultimate_attack;
}

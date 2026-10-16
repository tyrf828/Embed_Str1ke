//===========================================================================
//! @file EnemyLockPoint.cpp
//! @brief 敵をロックする位置
//===========================================================================
#include "EnemyLockPoint.h"
#include <Game/Object/Enemy/EnemyBase.h>
#include <System/Component/ComponentCollisionSphere.h>

//! @brief 初期化
//! @return 初期化済み
bool EnemyLockPoint::Init()
{
    Super::Init();
    return true;
}

//! @brief オーナーオを設定する
//! @param owner 設定するオーナー
void EnemyLockPoint::SetOwner(EnemyBasePtr owner)
{
    owner_ = owner;
}

//! @brief オーナーオを取得する
EnemyBasePtr EnemyLockPoint::GetOwner()
{
    return owner_.lock();
}

//! @brief オーナーが存在するか
//! @retval true 存在する
//! @retval false 存在しない
bool EnemyLockPoint::HaveOwner()
{
    if(auto owner = owner_.lock())
    {
        return owner->GetStatus(Object::StatusBit::Alive);
    }
    return false;
}

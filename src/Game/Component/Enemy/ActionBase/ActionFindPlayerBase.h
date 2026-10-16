#pragma once
//===========================================================================
//! @file ActionFindPlayerBase.h
//! @brief 敵のプレイヤー探索アクション基底クラス
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class ActionFindPlayerBase
//! @brief 敵のプレイヤー探索アクション基底クラス
//---------------------------------------------------------------------------
class ActionFindPlayerBase : public EnemyActionBase
{
protected:
    float3 target_pos_ = float3{0.0f, 0.0f, 0.0f};   //!< ターゲット位置
};

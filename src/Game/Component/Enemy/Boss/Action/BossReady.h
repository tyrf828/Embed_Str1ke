#pragma once
//===========================================================================
//! @file BossReady.h
//! @brief ボスの戦闘準備コンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class BossReady
//! @brief ボスの待機コンポーネント(演出)
//! @details ボス戦開始時の演出などに使用、プレイヤの移動を無効化する
//---------------------------------------------------------------------------
class BossReady : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(BossReady, u8"ボスの戦闘準備コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

private:
};

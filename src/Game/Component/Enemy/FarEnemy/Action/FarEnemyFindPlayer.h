#pragma once
//===========================================================================
//! @file FarEnemyFindPlayer.h
//! @brief 敵がプレイヤーを探すコンポーネント
//===========================================================================
#include <Game/Component/Enemy/ActionBase/ActionFindPlayerBase.h>

//---------------------------------------------------------------------------
//! @class FarEnemyFindPlayer
//! @brief 敵がプレイヤーを探すコンポーネント
//---------------------------------------------------------------------------
class FarEnemyFindPlayer : public ActionFindPlayerBase
{
public:
    BP_COMPONENT_DECL(FarEnemyFindPlayer, u8"プレイヤーを探すコンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    void LateDraw() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

private:
    static constexpr float WALK_SPEED_ = 0.65f;
};

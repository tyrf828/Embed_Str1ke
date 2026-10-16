#pragma once
//===========================================================================
//! @file CloseEnemyFindPlayer.h
//! @brief 敵がプレイヤーを探すコンポーネント
//===========================================================================
#include <Game/Component/Enemy/ActionBase/ActionFindPlayerBase.h>

//---------------------------------------------------------------------------
//! @class CloseEnemyFindPlayer
//! @brief 敵がプレイヤーを探すコンポーネント
//---------------------------------------------------------------------------
class CloseEnemyFindPlayer : public ActionFindPlayerBase
{
public:
    BP_COMPONENT_DECL(CloseEnemyFindPlayer, u8"プレイヤーを探すコンポーネント");

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
    bool is_in_player_range_           = false;   //!< プレイヤー範囲内フラグ
    static constexpr float WALK_SPEED_ = 0.7f;
};

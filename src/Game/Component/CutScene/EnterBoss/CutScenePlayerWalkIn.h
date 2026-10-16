#pragma once
//===========================================================================
//! @file CutScenePlayerWalkIn.h
//! @brief プレイヤーが入場するカットシーン
//===========================================================================
#include "Game/Component/CutScene/CutScene.h"

//----------------------------------------------------------
//! @class CutScenePlayerWalkIn
//! @brief プレイヤーが入場するカットシーン
//----------------------------------------------------------
class CutScenePlayerWalkIn : public CutSceneBase
{
public:
    BP_COMPONENT_DECL(CutScenePlayerWalkIn, u8"プレイヤー入場");

    //! @brief 初期化
    //! @param info カットシーンの保護情報
    void Init(const ProtectInfo& info) override;

    //! @brief 更新
    void Update() override;

    //! @brief GUI表示
    void GUI() override;

    //! @brief 終了処理
    void Exit() override;

private:
    static constexpr float CHANGE_NEXT_TIME_ = 4.0f;   //!< 次のカットシーンに遷移する時間
};

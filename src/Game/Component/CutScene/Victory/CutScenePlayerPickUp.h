#pragma once
//===========================================================================
//! @file CutScenePlayerPickUp.h
//! @brief プレイヤーがアイテムを拾うカットシーン
//===========================================================================
#include "Game/Component/CutScene/CutScene.h"

//----------------------------------------------------------
//! @class CutScenePlayerPickUp
//! @brief プレイヤーがアイテムを拾うカットシーン
//----------------------------------------------------------
class CutScenePlayerPickUp : public CutSceneBase
{
public:
    BP_COMPONENT_DECL(CutScenePlayerPickUp, u8"プレイヤーがアイテムを拾う");

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
    static constexpr float CHANGE_NEXT_TIME_   = 5.0f;    //!< 次のカットシーンに遷移する時間
    static constexpr float INIT_PERSPECTIVE_   = 25.0f;   //!< 初期の視野角
    static constexpr float TARGET_PERSPECTIVE_ = 40.0f;   //!< 目標の視野角
};

#pragma once
//===========================================================================
//! @file CutSceneFight.h
//! @brief プレイヤーとボスの対峙カットシーン
//===========================================================================
#include "Game/Component/CutScene/CutScene.h"

//----------------------------------------------------------
//! @class CutSceneFight
//! @brief プレイヤーとボスの対峙カットシーン
//----------------------------------------------------------
class CutSceneFight : public CutSceneBase
{
public:
    BP_COMPONENT_DECL(CutSceneFight, u8"プレイヤーとボスの対峙");

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
    static constexpr float CHANGE_NEXT_TIME_ = 3.0f;   //!< 次のシーンに遷移する時間
};

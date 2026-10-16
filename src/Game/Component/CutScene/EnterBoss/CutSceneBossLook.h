#pragma once
//===========================================================================
//! @file CutSceneBossLook.h
//! @brief ボスが遠くを見るカットシーン
//===========================================================================
#include "Game/Component/CutScene/CutScene.h"

//----------------------------------------------------------
//! @class CutSceneBossLook
//! @brief ボスが遠くを見るカットシーン
//----------------------------------------------------------
class CutSceneBossLook : public CutSceneBase
{
public:
    BP_COMPONENT_DECL(CutSceneBossLook, u8"ボスが遠く見る");

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
    static constexpr float CHANGE_NEXT_TIME_ = 3.0f;   //!< 次のカットシーンに遷移する時間
};

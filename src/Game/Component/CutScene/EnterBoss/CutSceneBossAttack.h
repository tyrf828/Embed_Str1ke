#pragma once
//===========================================================================
//! @file CutSceneBossAttack.h
//! @brief ボスが攻撃するカットシーン
//===========================================================================
#include "Game/Component/CutScene/CutScene.h"

//----------------------------------------------------------
//! @class CutSceneBossAttack
//! @brief ボスが攻撃するカットシーン
//----------------------------------------------------------
class CutSceneBossAttack : public CutSceneBase
{
public:
    BP_COMPONENT_DECL(CutSceneBossAttack, u8"ボスが攻撃");

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
    const float3 BOSS_START_POS_             = {-774.0f, 170.0f, 132.0f};   //!< ボスの開始位置
    static constexpr float CAMERA_FOV_       = 30.0f;                       //!< カメラの初期視野角
    static constexpr float CAMERA_FINAL_FOV_ = 65.0f;                       //!< カメラの最終視野角
    float camera_time_                       = 0.0f;                        //!< カメラの経過時間
};

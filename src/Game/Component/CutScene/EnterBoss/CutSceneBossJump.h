#pragma once
//===========================================================================
//! @file CutSceneBossJump.h
//! @brief ボスがジャンプするカットシーン
//===========================================================================
#include "Game/Component/CutScene/CutScene.h"

//----------------------------------------------------------
//! @class CutSceneBossJump
//! @brief ボスがジャンプするカットシーン
//----------------------------------------------------------
class CutSceneBossJump : public CutSceneBase
{
public:
    BP_COMPONENT_DECL(CutSceneBossJump, u8"ボスがジャンプ");

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
    static constexpr float CAMERA_STOP_TIME = 3.0f;                  //!< カメラが停止する時間
    float3 camera_move_dir_                 = {0.0f, 0.0f, -1.0f};   //!< カメラの移動方向
};

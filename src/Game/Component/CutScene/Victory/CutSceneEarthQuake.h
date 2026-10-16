#pragma once
//===========================================================================
//! @file CutSceneEarthQuake.h
//! @brief 地震のカットシーン
//===========================================================================
#include "Game/Component/CutScene/CutScene.h"

//----------------------------------------------------------
//! @class CutSceneEarthQuake
//! @brief 地震のカットシーン
//----------------------------------------------------------
class CutSceneEarthQuake : public CutSceneBase
{
public:
    BP_COMPONENT_DECL(CutSceneEarthQuake, u8"地震");

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
    bool is_camera_shaked_                              = false;                //!< カメラが揺れているかどうか
    float shake_time_                                   = 0.0f;                 //!< 揺れ時間
    static constexpr float POWER_                       = 0.5f;                 //!< 揺れの強さ
    float3 original_camera_pos_                         = {0.0f, 0.0f, 0.0f};   //!< カメラの元の位置
    static constexpr float TARGET_PERSPECTIVE_          = 55.0f;                //!< 目標の視野角
    float initial_perspective_                          = 0.0f;                 //!< 初期の視野角
    float perspective_time_                             = 0.0f;                 //!< 視野角変更の経過時間
    static constexpr float PERSPECTIVE_TRANSITION_TIME_ = 5.0f;                 //!< 視野角変更にかかる時間
};

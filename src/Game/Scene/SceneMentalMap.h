//===========================================================================
//! @file SceneMentalMap.h
//! @brief メンタルマップシーン
//===========================================================================
#pragma once

//---------------------------------------------------------------------------
//! @class SceneMentalMap
//! @brief メンタルマップ作成用シーン
//---------------------------------------------------------------------------
class SceneMentalMap : public Scene::Base
{
public:
    BP_CLASS_DECL(SceneMentalMap, u8"SceneMentalMap");

    //! @brief 初期化
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 更新後
    void PostUpdate() override;

    void Draw() override;

    //! @brief 描画
    void LateDraw() override;

    //! @brief 終了
    void Exit() override;

private:
    float3 nav_player_pos_  = float3{0.0f, 0.0f, 0.0f};   // ナビメッシュ上のプレイヤー位置
    float2 mouse_xy_        = {0, 0};                     // マウス位置
    float2 mouse_vec_       = {0, 0};                     // マウス移動量
    bool change_enemy_mode_ = false;                      // 敵を配置するモードか
};

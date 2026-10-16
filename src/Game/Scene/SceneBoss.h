//===========================================================================
//! @file SceneBoss.h
//! @brief ボス戦シーン
//===========================================================================
#pragma once

class FilterVolumetricFog;

//---------------------------------------------------------------------------
//! @class SceneBoss
//! @brief ボス戦シーン
//---------------------------------------------------------------------------
class SceneBoss : public Scene::Base
{
public:
    BP_CLASS_DECL(SceneBoss, u8"SceneBoss");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 終了
    void Exit() override;

private:
    std::weak_ptr<FilterVolumetricFog> volumetric_fog_;   //!< フォグ

    float scene_light_time_                  = 0.0f;   //!< ライトつけるタイマー
    static constexpr float LIGHT_SPAWN_TIME_ = 5.0f;   //!< ライト生成時間
    static constexpr float TOP_LIGHT_DELAY_  = 0.5f;   //!< 上のライト生成遅延時間
    std::array<bool, 2> is_light_spawned_;             //!< ライト生成済みフラグ

    //! @brief 下のライト生成
    void SpawnBottomLightPoint();
    //! @brief 上のライト生成
    void SpawnTopLightPoint();
};

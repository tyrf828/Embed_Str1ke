//===========================================================================
//! @file SceneTutorial.h
//! @brief チュートリアルシーン
//===========================================================================
#pragma once

class FilterVolumetricFog;

//---------------------------------------------------------------------------
//! @class SceneTutorial
//! @brief チュートリアルシーン
//---------------------------------------------------------------------------
class SceneTutorial : public Scene::Base
{
public:
    BP_CLASS_DECL(SceneTutorial, u8"SceneTutorial");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 終了
    void Exit() override;

private:
    std::weak_ptr<FilterVolumetricFog> volumetric_fog_;   //!< フォグ
};

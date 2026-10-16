//===========================================================================
//! @file SceneGame.h
//! @brief ゲームシーン
//===========================================================================
#pragma once

class FilterVolumetricFog;

//---------------------------------------------------------------------------
//! @class SceneGame
//! @brief ゲームシーン
//---------------------------------------------------------------------------
class SceneGame : public Scene::Base
{
public:
    BP_CLASS_DECL(SceneGame, u8"SceneGame");

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

//===========================================================================
//! @file SceneVictoryCutScene.h
//! @brief プレイヤ勝利カットシーン
//===========================================================================
#pragma once

//---------------------------------------------------------------------------
//! @class SceneVictoryCutScene
//! @brief プレイヤ勝利カットシーン
//---------------------------------------------------------------------------
class SceneVictoryCutScene : public Scene::Base
{
public:
    BP_CLASS_DECL(SceneVictoryCutScene, u8"SceneVictoryCutScene");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

private:
};

//===========================================================================
//! @file SceneCutScene.h
//! @brief ボスカットシーン
//===========================================================================
#pragma once

//---------------------------------------------------------------------------
//! @class SceneCutScene
//! @brief ボスカットシーン
//---------------------------------------------------------------------------
class SceneCutScene : public Scene::Base
{
public:
    BP_CLASS_DECL(SceneCutScene, u8"SceneCutScene");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

private:
};

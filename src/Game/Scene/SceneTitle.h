//===========================================================================
//! @file SceneTitle.h
//! @brief タイトルシーン
//===========================================================================
#pragma once

//---------------------------------------------------------------------------
//! @class SceneTitle
//! @brief タイトルシーン
//---------------------------------------------------------------------------
class SceneTitle : public Scene::Base
{
public:
    BP_CLASS_DECL(SceneTitle, u8"SceneTitle");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

private:
    //! @brief プレイヤーデータリセット
    void ResetPlayerData();
};

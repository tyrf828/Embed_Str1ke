#pragma once

//! @brief ゲーム中シーン
class SceneTest : public Scene::Base
{
public:
    // ALT-Menuで表示される名前
    BP_CLASS_DECL(SceneTest, u8"SceneTest");

    //! @brief 初期化
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 更新後
    void PostUpdate() override;

    //! @brief 描画
    void LateDraw() override;

    //! @brief 終了
    void Exit() override;

private:
    static constexpr int GOAL_MAX_ = 8;
    float3 goal_pos_[GOAL_MAX_];
};

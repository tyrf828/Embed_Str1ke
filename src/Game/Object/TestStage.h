#pragma once

USING_PTR(TestStage);

class TestStage : public Object
{
public:
    BP_OBJECT_DECL(TestStage, "TestStage");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    void PostUpdate() override;

    void Exit() override;

private:
};

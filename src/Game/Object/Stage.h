#pragma once
//===========================================================================
//! @file Stage.h
//! @brief ステージ
//===========================================================================

USING_PTR(Stage);

//---------------------------------------------------------------------------
//! @class Stage
//! @brief ステージ
//---------------------------------------------------------------------------
class Stage : public Object
{
public:
    BP_OBJECT_DECL(Stage, "Stage");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 更新後処理
    void PostUpdate() override;

    //! @brief 終了
    void Exit() override;

private:
};

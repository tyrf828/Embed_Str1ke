#pragma once
//===========================================================================
//! @file BossStage.h
//! @brief ボスステージ
//===========================================================================

USING_PTR(BossStage);

//---------------------------------------------------------------------------
//! @class BossStage
//! @brief ボスステージ
//---------------------------------------------------------------------------
class BossStage : public Object
{
public:
    BP_OBJECT_DECL(BossStage, "BossStage");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 終了
    void Exit() override;

private:
};

#pragma once
//===========================================================================
//! @file BossSkyDome.h
//! @brief スカイドーム
//===========================================================================

USING_PTR(BossSkyDome);
//---------------------------------------------------------------------------
//! @class BossSkyDome
//! @brief スカイドーム
//---------------------------------------------------------------------------
class BossSkyDome : public Object
{
public:
    BP_OBJECT_DECL(BossSkyDome, "BossSkyDome");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

private:
};

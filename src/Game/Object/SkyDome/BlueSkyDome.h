#pragma once
//===========================================================================
//! @file BlueSkyDome.h
//! @brief スカイドーム
//===========================================================================

USING_PTR(BlueSkyDome);
//---------------------------------------------------------------------------
//! @class BlueSkyDome
//! @brief
//---------------------------------------------------------------------------
class BlueSkyDome : public Object
{
public:
    BP_OBJECT_DECL(BlueSkyDome, "BlueSkyDome");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

private:
};

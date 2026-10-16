#pragma once
//===========================================================================
//! @file ComponentHP.h
//! @brief HPのベースコンポーネント
//===========================================================================

USING_PTR(ComponentHP);

//---------------------------------------------------------------------------
//! @class ComponentHP
//! @brief HPのベースコンポーネント
//---------------------------------------------------------------------------
class ComponentHP : public Component
{
public:
    BP_COMPONENT_DECL(ComponentHP, u8"HPコンポーネント");

    void GUI() override;

    //! @brief HPを取得
    //! @return HP
    int GetHP() const;

    //! @brief HPを減らす
    //! @param HPを減らす設定量
    void SubHP(int hp);

    //! @brief HPを増やす
    void AddHP(int hp);

    //! @brief HPを設定
    //! @param hp HPの設定量
    ComponentHPPtr SetHP(int hp);

    //! @brief 最大HPを設定
    //! @param max_hp 最大HP
    ComponentHPPtr SetMaxHP(int max_hp);

    //! @brief 最大HPを取得
    //! @return 最大HP
    int GetMaxHP() const;

protected:
    int hp_     = 0;   //!< HP
    int max_hp_ = 0;   //!< 最大HP
};

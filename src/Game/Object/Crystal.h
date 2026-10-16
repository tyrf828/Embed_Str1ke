#pragma once
//===========================================================================
//! @file Crystal.h
//! @brief クリスタル
//===========================================================================

//---------------------------------------------------------------------------
//! @class Crystal
//! @brief クリスタル
//---------------------------------------------------------------------------
class Crystal : public Object
{
public:
    BP_OBJECT_DECL(Crystal, u8"クリスタル");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 衝突時の処理
    //! @param hit_info 衝突したオブジェクトの情報
    void OnHit(const ComponentCollision::HitInfo& hit_info) override;

private:
};

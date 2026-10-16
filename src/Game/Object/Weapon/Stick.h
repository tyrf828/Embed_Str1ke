#pragma once
//===========================================================================
//! @file Stick.h
//! @brief プレイヤーの武器
//===========================================================================
USING_PTR(Stick);

//---------------------------------------------------------------------------
//! @class Stick
//! @brief 棒
//---------------------------------------------------------------------------
class Stick : public Object
{
public:
    BP_OBJECT_DECL(Stick, "Stick");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 衝突時の処理
    //! @param hit_info 衝突したオブジェクトの情報
    void OnHit(const ComponentCollision::HitInfo& hit_info) override;

private:
};

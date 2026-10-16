//===========================================================================
//! @file AttachWeaponBase.h
//! @brief プレイヤーの武器基底クラス
//===========================================================================
#pragma once
USING_PTR(AttachWeaponBase);

//---------------------------------------------------------------------------
//! @class AttachWeaponBase
//! @brief プレイヤーの武器基底クラス
//---------------------------------------------------------------------------
class AttachWeaponBase : public Object
{
public:
    BP_OBJECT_DECL(AttachWeaponBase, "AttachWeaponBase");

    //! @brief 初期化
    //! @return 初期化済み
    virtual bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 遅い描画
    void LateDraw() override;

    //! @brief 衝突時の処理
    //! @param hit_info 衝突したオブジェクトの情報
    void OnHit(const ComponentCollision::HitInfo& hit_info) override;

    //! @brief オブジェクト情報のリセット
    void ResetObjectInfo() override;

protected:
    struct Collision_Data
    {
        float radius_ = 0.0f;   //!< 半径
        float height_ = 0.0f;   //!< 高さ
        float mass_   = 0.0f;   //!< 質量
    };
    Collision_Data collision_data_;          //!< コリジョンデータ
    int attach_key_ghandle_[2] = {-1, -1};   //!< アタッチ提示画像ハンドル
};

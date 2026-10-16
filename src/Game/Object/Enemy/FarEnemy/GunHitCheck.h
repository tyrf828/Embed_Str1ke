#pragma once
//===========================================================================
//! @file GunHitCheck.h
//! @brief 敵の武器の射撃判定
//===========================================================================

USING_PTR(GunHitCheck);
USING_PTR(EnemyHitEffect);

//---------------------------------------------------------------------------
//! @class GunHitCheck
//! @brief 敵の武器の射撃判定
//---------------------------------------------------------------------------
class GunHitCheck : public Object
{
public:
    BP_OBJECT_DECL(GunHitCheck, u8"敵の武器の射撃判定");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 衝突時の処理
    //! @param hit_info 衝突したオブジェクトの情報
    void OnHit(const ComponentCollision::HitInfo& hit_info) override;

    //! @brief 終了
    void Exit() override;

    //! @brief オーナーを設定
    //! @param owner オーナー
    void SetOwner(ObjectPtr owner)
    {
        owner_ = owner;
    }

    //! @brief 攻撃距離を設定
    //! @param length 距離
    void SetAttackLength(float length);

private:
    ObjectWeakPtr owner_;                                    //!< オーナー
    std::shared_ptr<EnemyHitEffect> hit_effect_ = nullptr;   //!< ヒットエフェクト
};

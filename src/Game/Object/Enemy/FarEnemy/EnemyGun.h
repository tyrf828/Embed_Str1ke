#pragma once
//===========================================================================
//! @file EnemyGun.h
//! @brief 敵の武器
//===========================================================================
USING_PTR(GunHitCheck);
USING_PTR(EnemyGun);
USING_PTR(ShootLaserEffect);

//---------------------------------------------------------------------------
//! @class EnemyGun
//! @brief 敵の武器
//---------------------------------------------------------------------------
class EnemyGun : public Object
{
public:
    BP_OBJECT_DECL(EnemyGun, u8"敵の武器");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief オーナーを設定
    //! @param owner オーナー
    void SetOwner(ObjectPtr owner)
    {
        owner_ = owner;
    }

    //! @brief モデルをアタッチ
    //! @param node アタッチするノード名
    void AttachModel(std::string_view node);

    //! @brief 攻撃判定を設定
    void SetHit();

    //! @brief 攻撃判定のコリジョンを設定
    void SetHitCollision();

    //! @brief 攻撃判定を削除
    void RemoveHit();

    //! @brief 攻撃距離を設定
    //! @param length 距離
    void HitLength(float length);

    //! @brief 攻撃判定のコリジョンを取得
    //! @return 攻撃判定のコリジョン
    GunHitCheckPtr GetHitCollision() const
    {
        return gun_hit_.lock();
    }

    //! @brief エフェクトを取得
    //! @return エフェクト
    ShootLaserEffectPtr GetEffect() const
    {
        return effect_.lock();
    }

private:
    //! @brief 銃口の位置を取得
    //! @param use_local ローカル座標を使用するか
    //! @return 銃口の位置
    float3 GetBarrelPos(bool use_local = false) const;

    ObjectWeakPtr owner_;                   //!< オーナー
    GunHitCheckWeakPtr gun_hit_;            //!< 攻撃判定
    ShootLaserEffectWeakPtr effect_;        //!< エフェクト
    bool is_shoot_               = false;   //!< 攻撃中か
    float current_attack_length_ = 0.0f;    //!< 現在の攻撃距離
};

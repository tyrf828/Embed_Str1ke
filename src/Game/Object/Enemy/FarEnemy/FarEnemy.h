#pragma once
//===========================================================================
//! @file FarEnemy.h
//! @brief 遠距離敵
//===========================================================================
#include <Game/Object/Enemy/EnemyBase.h>
#include <Game/Object/Enemy/FarEnemy/EnemyGun.h>

USING_PTR(EnemyLockPoint);
USING_PTR(FarEnemy);
USING_PTR(EnemyGun);

//---------------------------------------------------------------------------
//! @class FarEnemy
//! @brief 遠距離敵
//---------------------------------------------------------------------------
class FarEnemy : public EnemyBase
{
public:
    BP_OBJECT_DECL(FarEnemy, "FarEnemy");

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

    //! @brief ロックオンポイントを作成
    //! @return ロックオンポイント
    EnemyLockPointPtr CreateEnemyLockPoint();

    //! @brief ロックポイントを作成せずにロック座標を取得
    //! @return ロックポイントのワールド座標
    float3 GetVirutalLockPointPos() override;

    //! @brief 武器を取得
    //! @return 武器
    EnemyGunPtr GetWeaponPtr() const;

    //! @brief オブジェクト情報をリセット
    void ResetObjectInfo() override;

private:
    EnemyGunPtr gun_             = nullptr;   //!< 武器
    static constexpr int MAX_HP_ = 300;       //!< 最大HP
};

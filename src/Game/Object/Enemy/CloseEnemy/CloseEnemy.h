#pragma once
//===========================================================================
//! @file CloseEnemy.h
//! @brief 近距離敵
//===========================================================================
#include <Game/Object/Enemy/EnemyBase.h>
#include <Game/Object/Enemy/EnemyLockPoint.h>
#include <Game/Object/Enemy/CloseEnemy/EnemySword.h>
USING_PTR(CloseEnemy);

//---------------------------------------------------------------------------
//! @class CloseEnemy
//! @brief 近距離敵
//---------------------------------------------------------------------------
class CloseEnemy : public EnemyBase
{
public:
    BP_OBJECT_DECL(CloseEnemy, "CloseEnemy");

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

    //! @brief ロックポイント作成
    EnemyLockPointPtr CreateEnemyLockPoint();

    //! @brief ロックポイントを作成せずにロック座標を取得
    //! @return ロックポイントの座標
    float3 GetVirutalLockPointPos() override;

    //! @brief 武器オブジェクト取得
    EnemySwordPtr GetWeaponPtr() const
    {
        return weapon_sword_;
    }

    //! @brief オブジェクト情報リセット
    void ResetObjectInfo() override;

private:
    EnemySwordPtr weapon_sword_  = nullptr;   //!< 武器オブジェクト
    static constexpr int MAX_HP_ = 500;       //!< 最大体力
};

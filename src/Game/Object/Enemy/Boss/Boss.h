#pragma once
//===========================================================================
//! @file Boss.h
//! @brief ボス
//===========================================================================
#include <Game/Object/Enemy/EnemyBase.h>
#include <Game/Object/Enemy/EnemyLockPoint.h>

USING_PTR(Boss);
//---------------------------------------------------------------------------
//! @class Boss
//! @brief ボス
//---------------------------------------------------------------------------
class Boss : public EnemyBase
{
public:
    BP_OBJECT_DECL(Boss, "Boss");

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

    //! @brief ロックオンポイントを作成する
    EnemyLockPointPtr CreateEnemyLockPoint();

    //! @brief ロックポイントを作成せずにロック座標を取得
    //! @return ロックポイントの座標
    float3 GetVirutalLockPointPos() override;

private:
    static constexpr int MAX_HP_ = 5000;                       //!< 最大体力
    float3 lock_point_offset_    = float3{0.0f, 0.0f, 0.0f};   //!< ロックポイントのオフセット
};

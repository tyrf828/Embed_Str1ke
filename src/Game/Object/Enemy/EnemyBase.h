#pragma once
//===========================================================================
//! @file EnemyBase.h
//! @brief 敵の基底クラス
//===========================================================================
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Object/Enemy/EnemyLockPoint.h>

USING_PTR(EnemyBase);

//---------------------------------------------------------------------------
//! @class EnemyBase
//! @brief 敵の基底クラス
//---------------------------------------------------------------------------
class EnemyBase : public Object
{
public:
    BP_OBJECT_DECL(EnemyBase, "EnemyBase");

    //! @brief 更新
    void Update() override;

    //! @brief 半径を取得
    //! @return 半径
    float GetRadius() const
    {
        if(auto col = GetComponent<ComponentCollisionCapsule>())
        {
            return col->GetRadius();
        }
        return 0.0f;
    }

    //! @brief ロックオンポイントを作成
    //! @return ロックオンポイント
    virtual EnemyLockPointPtr CreateEnemyLockPoint()
    {
        return nullptr;
    }

    //! @brief ロックポイントを作成せずにロック座標を取得
    //! @return ロックポイントの座標
    virtual float3 GetVirutalLockPointPos()
    {
        return GetTranslate();
    }

    //! @brief 自分のロックオンポイントを取得する
    //! @brief プレイヤーのカメラが見る場所
    //! @return 自分のロックポイント
    EnemyLockPointPtr GetEnemyLockPoint()
    {
        return lock_point_;
    }

    //! @brief ロックオンポイントをクリア
    void ClearEnemyLockPoint()
    {
        if(lock_point_)
        {
            lock_point_->RemoveAllComponents();
            Scene::Object::Release(lock_point_);
            lock_point_ = nullptr;
        }
    }

    //! @brief 無敵時間を設定
    //! @param time 無敵時間
    void SetInvincibleTime(float time)
    {
        invincible_time_ = time;
    }

    //! @brief 無敵かどうか
    //! @retval true 無敵
    //! @retval false 無敵ではない
    bool IsInvincible() const;

protected:
    EnemyLockPointPtr lock_point_ = nullptr;   //!< カメラのロックポイント
    float invincible_time_        = 0.0f;      //!< 無敵時間
    struct CollisionInfo
    {
        float radius_ = 0.0f;   //!< 半径
        float height_ = 0.0f;   //!< 高さ
        float mass_   = 0.0f;   //!< 質量
    };
    CollisionInfo collision_info_;   //!< コリジョン情報

public:
    //! @brief コリジョンをリセット
    void ResetCollision();

    //! @brief コリジョン情報を取得
    //! @return コリジョン情報
    CollisionInfo GetCollisionInfo() const
    {
        return collision_info_;
    }

    //! @brief オブジェクト情報をリセット
    void ResetObjectInfo() override;
};

#pragma once
//===========================================================================
//! @file EnemyLockPoint.h
//! @brief 敵をロックする位置
//===========================================================================

USING_PTR(EnemyLockPoint);
USING_PTR(EnemyBase);

//---------------------------------------------------------------------------
//! @class EnemyLockPoint
//! @brief 敵をロックする位置
//---------------------------------------------------------------------------
class EnemyLockPoint : public Object
{
public:
    BP_OBJECT_DECL(EnemyLockPoint, "EnemyLockPoint");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief オーナーオを設定する
    //! @param owner 設定するオーナー
    void SetOwner(EnemyBasePtr owner);

    //! @brief オーナーオを取得する
    //! @return オーナー
    EnemyBasePtr GetOwner();

    //! @brief オーナーが存在するか
    //! @retval true 存在する
    //! @retval false 存在しない
    bool HaveOwner();

private:
    EnemyBaseWeakPtr owner_;   //!< ロックポイントのオーナー
};

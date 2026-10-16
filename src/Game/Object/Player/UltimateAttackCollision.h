//===========================================================================
//! @file UltimateAttackCollision.h
//! @brief 必殺技の衝突判定
//===========================================================================
#pragma once

USING_PTR(UltimateAttackCollision);
USING_PTR(EnemyBase);

class UltimateAttackCollision : public Object
{
public:
    BP_OBJECT_DECL(UltimateAttackCollision, "UltimateAttackCollision");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新処理
    void Update() override;

    //! @brief 衝突時の処理
    //! @param hit_info 衝突したオブジェクトの情報
    void OnHit(const ComponentCollision::HitInfo& hit_info) override;

    //! @brief 終了処理
    void Exit() override;

    //! @brief 攻撃範囲の設定
    void SetAttackRadius(float radius);

private:
    std::unordered_set<Object*> enemy_vec_;   //!< 敵
    EnemyBasePtrVec enemy_base_vec_;          //!< 敵のポインタベクタ
};

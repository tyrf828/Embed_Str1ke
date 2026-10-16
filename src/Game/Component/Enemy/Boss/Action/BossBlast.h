#pragma once
//===========================================================================
//! @file BossBlast.h
//! @brief ボスの爆発攻撃コンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

USING_PTR(BossBlastEffect);

//---------------------------------------------------------------------------
//! @class BossBlast
//! @brief ボスの爆発攻撃コンポーネント
//---------------------------------------------------------------------------
class BossBlast : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(BossBlast, u8"ボスの爆発攻撃コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 衝突時
    //! @param hit_info 衝突情報
    void OnHit(const ComponentCollision::HitInfo& hit_info) override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

private:
    static constexpr float ATTACK_START_TIME_      = 3.44f;     //!< 攻撃判定の開始時間
    static constexpr float ATTACK_END_TIME_        = 3.62f;     //!< 攻撃判定の終了時間
    static constexpr float ATTACK_RADIUS_          = 40.0f;     //!< 攻撃範囲
    static constexpr int ATTACK_POINT_             = 150;       //!< 攻撃力
    std::shared_ptr<BossBlastEffect> blast_effect_ = nullptr;   //!< 爆発エフェクト
};

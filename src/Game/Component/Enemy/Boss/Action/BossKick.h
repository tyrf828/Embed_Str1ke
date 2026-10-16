#pragma once
//===========================================================================
//! @file BossKick.h
//! @brief ボスのキック攻撃コンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

USING_PTR(BossHitEffect);

//---------------------------------------------------------------------------
//! @class BossKick
//! @brief ボスのキック攻撃コンポーネント
//---------------------------------------------------------------------------
class BossKick : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(BossKick, u8"ボスのキックコンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 衝突時
    void OnHit(const ComponentCollision::HitInfo& hit_info) override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

private:
    static constexpr float ATTACK_START_TIME_  = 1.24f;     //!< 攻撃判定の開始時間
    static constexpr float ATTACK_END_TIME_    = 1.85f;     //!< 攻撃判定の終了時間
    static constexpr float ATTACK_DISTANCE_    = 15.0f;     //!< 攻撃距離
    static constexpr int ATTACK_POINT_         = 130;       //!< 攻撃力
    std::shared_ptr<BossHitEffect> hit_effect_ = nullptr;   //!< ヒットエフェクト
};

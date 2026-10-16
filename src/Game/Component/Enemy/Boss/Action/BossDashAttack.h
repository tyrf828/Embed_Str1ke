#pragma once
//===========================================================================
//! @file BossDashAttack.h
//! @brief ボスの突進攻撃
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

USING_PTR(BossHitEffect);

//---------------------------------------------------------------------------
//! @class BossDashAttack
//! @brief ボスの突進攻撃コンポーネント
//---------------------------------------------------------------------------
class BossDashAttack : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(BossDashAttack, u8"ボスの突進攻撃コンポーネント");

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
    float3 init_pos_                           = float3{0.0f, 0.0f, 0.0f};   //!< 初期位置
    float3 target_pos_                         = float3{0.0f, 0.0f, 0.0f};   //!< 目標位置
    float time_                                = 0.0f;                       //!< 突進移動時間
    bool slow_motion_                          = false;                      //!< スローモーション中か
    float slow_elapsed_time_                   = 0.0f;                       //!< スローモーション経過時間
    static constexpr float ATTACK_START_TIME_  = 2.53f;                      //!< 攻撃判定の開始時間
    static constexpr float ATTACK_END_TIME_    = 3.01f;                      //!< 攻撃判定の終了時間
    static constexpr int ATTACK_POINT_         = 220;                        //!< 攻撃力
    std::shared_ptr<BossHitEffect> hit_effect_ = nullptr;                    //!< ヒットエフェクト
};

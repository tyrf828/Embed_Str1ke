#pragma once
//===========================================================================
//! @file BossGrabAttack.h
//! @brief ボスの掴み攻撃コンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>
#include <System/Component/ComponentCollision.h>

USING_PTR(BossHitEffect);

//---------------------------------------------------------------------------
//! @class BossGrabAttack
//! @brief ボスの掴み攻撃コンポーネント
//---------------------------------------------------------------------------
class BossGrabAttack : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(BossGrabAttack, u8"ボスの掴み攻撃コンポーネント");

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
    float3 init_pos_                           = float3{0.0f, 0.0f, 0.0f};   //!< 初期位置
    float3 target_pos_                         = float3{0.0f, 0.0f, 0.0f};   //!< ターゲット位置
    float time_                                = 0.0f;                       //!< 経過時間
    float max_distance_                        = 50.0f;                      //!< 攻撃の最大距離
    static constexpr float SLOW_TIME_          = 0.73f;                      //!< スローモーションまでの時間
    static constexpr float ATTACK_START_TIME_  = 2.08f;                      //!< 攻撃判定の開始時間
    static constexpr float ATTACK_END_TIME_    = 2.32f;                      //!< 攻撃判定の終了時間
    static constexpr float ANIMATION_END_TIME_ = 4.28f;                      //!< アニメーション終了時間
    static constexpr int ATTACK_POINT_         = 180;                        //!< 攻撃力
    std::shared_ptr<BossHitEffect> hit_effect_ = nullptr;                    //!< ヒットエフェクト
};

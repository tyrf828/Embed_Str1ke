#pragma once
//===========================================================================
//! @file BossActionManager.h
//! @brief ボスの動作管理マネージャー
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class BossActionManager
//! @brief ボスの動作管理マネージャー
//---------------------------------------------------------------------------
class BossActionManager : public Component
{
public:
    BP_COMPONENT_DECL(BossActionManager, u8"ボスの動作管理マネージャー");

    //! @brief 攻撃タイプ
    enum AttackType
    {
        KICK,            //!< キック
        GRAB,            //!< 掴み
        DASH,            //!< 突進
        FALLBACK_DASH,   //!< 後退からの突進
        BLAST,           //!< 爆発

        MAX,   //!< 最大
        NONE   //!< 無し
    };

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 前回の攻撃開始以降に、実際にダメージを受けた回数を加算
    void AddHitNumCount()
    {
        hit_num_count++;
    }

    //! @brief 攻撃成功時の処理
    //! @param attack_type 成功した攻撃タイプ
    void OnAttackSuccess(AttackType attack_type);

    //! @brief 攻撃失敗時の処理
    //! @param attack_type 失敗した攻撃タイプ
    void OnAttackFailed(AttackType attack_type);

    //! @brief 最後の攻撃タイプを取得
    //! @return 最後の攻撃タイプ
    AttackType GetAttackType() const
    {
        return last_attack_;
    }

private:
    float player_distance_ = 0.0f;    //!< プレイヤーとの距離
    bool player_is_regen_  = false;   //!< プレイヤーが回復中か

    static constexpr float THINK_TIME_MAX_ = 3.0f;   //!< 採点間隔
    float think_timer_                     = 0.0f;   //!< 採点タイマー
    int hit_num_count                      = 0;      //!< 攻撃開始時にリセットする被弾回数

    //-----------------------------------------------------------
    //! @struct Attack
    //! @brief 攻撃情報
    //-----------------------------------------------------------
    struct Attack
    {
        AttackType type_;
        float score_;
    };

    std::array<Attack, AttackType::MAX> attack_;   //!< 攻撃種類

    AttackType last_attack_ = AttackType::KICK;   //!< 最後の攻撃

    //-----------------------------------------------------------
    //! @name スコア計算
    //-----------------------------------------------------------
    //! @{

    float ScoreKick();
    float ScoreGrab();
    float ScoreDash();
    float ScoreFallBack();
    float ScoreBlast();

    //! @}

    //! @brief 実行
    void Execute(Attack& attack);

    //! @brief 採点用情報更新
    void UpdateContext();

    //! @brief 指定した型のアクション中なのか
    //! @tparam T チェックするアクションの型
    //! @retval true 指定した型のアクション中
    //! @retval false 指定した型のアクション中ではない
    template <typename T> bool IsAction(const std::shared_ptr<EnemyActionBase>& action)
    {
        return std::dynamic_pointer_cast<T>(action) != nullptr;
    }

    //! @brief -1.0から1.0にクランプ
    float ClampM1P1(float value);

    //-----------------------------------------------------------
    //! @name スコア計算基準
    //-----------------------------------------------------------
    //! @{

    //! @brief 距離スコア計算
    //! @param distance 高得点の距離
    //! @param range 高得点の距離範囲
    //! @param penalty_rate 範囲から離れた衰減率
    float DistanceScore(float distance, float range, float penalty_rate = 0.1f);

    //! @brief HPタイプ
    enum HP_TYPE
    {
        HP_HIGH = 100,   //!< 高
        HP_MID  = 75,    //!< 中
        HP_LOW  = 40,    //!< 低

        HP_TYPE_MAX = 3
    };

    //! @brief ボスHPスコア計算
    //! @param hp_type HPタイプ
    float BossHPScore(HP_TYPE hp_type);

    //! @brief プレイヤーHPスコア計算
    //! @param hp_type HPタイプ
    float PlayerHPScore(HP_TYPE hp_type);

    //! @brief 前回の攻撃開始以降の被弾
    float GettingHitScore(float rate);

    //! @brief プレイヤー回復スコア計算
    float PlayerRegenerateScore();

    std::array<float, AttackType::MAX> attack_success_bouns_;   //!< 攻撃成功ボーナスと失敗のペナルティ

    //! @}

    bool use_onnx_ai_ = true;   //!< ONNX AIを使用するか
};

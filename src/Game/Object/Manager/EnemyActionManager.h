#pragma once
//===========================================================================
//! @file EnemyActionManager.h
//! @brief 敵の行動管理マネージャー
//===========================================================================
#include <Game/Object/Enemy/EnemyBase.h>
#include <Game/Object/Player/Camera.h>
#include <unordered_map>

USING_PTR(EnemyActionManager);
//----------------------------------------------------------
//! @class EnemyActionManager
//! @brief 敵の行動管理
//----------------------------------------------------------
class EnemyActionManager : public Object
{
public:
    BP_OBJECT_DECL(EnemyActionManager, "EnemyActionManager");

    //! @brief シングルトン取得
    static EnemyActionManager* GetInstance()
    {
        return instance_;
    }

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 終了
    void Exit() override;

    //! @brief 攻撃を停止するか設定
    //! @param is_stop 停止するか
    //! @param clear_attack_enemy 攻撃候補の敵をクリアするか
    void StopAttack(bool is_stop, bool clear_attack_enemy = true);

private:
    static constexpr int ENEMY_ACTION_NUM_MAX_ = 3;      //!< 同時に攻撃できる敵の最大数
    static constexpr float CHANGE_TIME_        = 6.0f;   //!< 攻撃対象を変更する時間間隔

    EnemyBasePtrVec enemy_vec_;   //!< 敵リスト

    float change_timer_ = 0.0f;    //!< 変更タイマー
    bool is_stop_       = false;   //!< 停止フラグ

    static EnemyActionManager* instance_;   //!< シングルトン

    //! @brief 攻撃遅延情報
    struct AttackDelayInfo
    {
        EnemyBasePtr enemy;   //!< 敵のポインタ
        float delay_time;     //!< 残り遅延時間
    };

    std::vector<AttackDelayInfo> attack_delay_list_;   //!< 攻撃遅延リスト
    static constexpr float ATTACK_DELAY_MIN_ = 0.3f;   //!< 攻撃開始の最小遅延時間
    static constexpr float ATTACK_DELAY_MAX_ = 1.0f;   //!< 攻撃開始の最大遅延時間

    //! @brief 最も近い威嚇状態の敵を攻撃状態に変更
    void ChangeNearestThreatEnemiesToAttack();
};

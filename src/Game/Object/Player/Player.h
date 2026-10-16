#pragma once
//===========================================================================
//! @file Player.h
//! @brief プレイヤー
//===========================================================================

USING_PTR(Player);

//----------------------------------------------------------
//! @class Player
//! @brief プレイヤーの操作と状態管理
//----------------------------------------------------------
class Player : public Object
{
public:
    BP_OBJECT_DECL(Player, "Player");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 更新後
    void PostUpdate() override;

    //! @brief 衝突時の処理
    //! @param hit_info 衝突したオブジェクトの情報
    void OnHit(const ComponentCollision::HitInfo& hit_info) override;

    //! @brief プレイヤーの前方の方向の平面ベクトルを取得
    //! @return プレイヤーの前方を示す方向平面ベクトル
    float3 GetPlaneFront();

    //! @brief モデルの前方方向平面ベクトルを取得
    //! @param model プレイヤーのモデル
    //! @return モデルの前方を示す方向平面ベクトル
    float3 GetModelPlaneFront(ComponentModelPtr model);

    //! @brief ヒット状態を取得
    //! @retval true 攻撃されている場合
    //! @retval false 攻撃されていない場合
    bool IsHit();

    //! @brief 行動可能かどうかを設定
    //! @return 行動可能かどうか
    void SetActionEnable(bool enable);

    //! @brief 回復しているかどうか
    //! @retval true 回復している場合
    //! @retval false 回復していない場合
    bool IsRegenerateHP() const;

    //! @brief 回復状態を設定
    //! @param regen 回復状態にするかどうか
    void SetRegenerateHP(bool regen);

    //! @brief HPボトルの残量を取得
    //! @return HPボトルの残量
    int GetHPBottleAmount() const;

    //! @brief HPボトルの残量を設定
    //! @param value 設定するHPボトルの数
    void SetHPBottleAmount(int value);

    //! @brief 最大HPボトル数を取得
    //! @return 最大HPボトル数
    int GetHPBottleMax() const;

    //! @brief プレイヤーのHPを設定
    //! @param hp 設定するHP値
    void SetHP(int hp)
    {
        hp_ = hp;
    }

    //! @brief 走っているかどうかを取得
    //! @return 走っているかどうか
    bool IsRun() const
    {
        return is_run_;
    }

private:
    struct KEY
    {
        int attack_key_ = 0;   //!< 攻撃
    };
    KEY key_;                      //!< キー設定
    bool action_enable_ = true;    //!< 行動可能かどうか
    bool is_hit_        = false;   //!< ダメージを受けたかどうか

    float3 hit_knockback_ = {0, 0, 0};   //!< 攻撃を受けたときのノックバック方向

    static constexpr int ATTACK_STAMINA_ = 100;    //!< 攻撃に必要なスタミナ
    static constexpr float MOVE_SPEED_   = 0.7f;   //!< 移動速度
    float key_frame_time_                = 0;      //!< 回避・走りのキーフレーム時間

    bool is_run_stop_                      = false;   //!< 走りを止めるかどうか
    bool is_run_                           = false;   //!< 走っているかどうか
    static constexpr float RUN_KEY_FRAME_  = 20.0f;   //!< 走る時のキーフレーム
    static constexpr int RUN_STAMINA_COST_ = 1;       //!< 走る時のスタミナ消費

    bool is_regen_hp_ = false;   //!< HP回復中かどうか
    bool is_ground_   = false;   //!< 地面に接地しているかどうか

    int hp_bottle_     = 0;   //!< HPアイテムの残量
    int HP_BOTTLE_MAX_ = 3;   //!< 最大HPアイテム数

    static int hp_;
    int max_hp_ = 1000;   //!< 最大HP

    //! @brief プレイヤーとオブジェクトとの角度を計算
    //! @param obj_ptr オブジェクト
    float PlayerToObjectDegree(ObjectPtr obj_ptr);

    //! @brief 他のアクションコンポーネントのチェック
    //! @retval true 他のアクションコンポーネントがある
    //! @retval false 他のアクションコンポーネントがいない
    bool CheckOtherActionComponent();

    //! @brief 走り状態のリセット
    void ResetRunState();

    //! @brief 被弾状態の処理
    void HitProcess();

    //! @brief 死亡状態の処理
    void DeathProcess();

    //! @brief 走りから歩きに戻るするかの処理
    void StopRunCheck();

    //! @brief 走りから歩きに戻る
    void StopRunProcess();
};

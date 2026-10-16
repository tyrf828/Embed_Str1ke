#pragma once
//===========================================================================
//! @file EnemyActionBase.h
//! @brief 敵の行動ベースクラス
//===========================================================================

class ComponentCollision;

//---------------------------------------------------------------------------
//! @class EnemyActionBase
//! @brief 敵の行動ベースクラス
//---------------------------------------------------------------------------
class EnemyActionBase : public Component
{
public:
    BP_COMPONENT_DECL(EnemyActionBase, u8"敵の行動ベースクラス");

    //! @brief オーナーを設定
    //! @param owner オーナーオブジェクト
    void SetOwner(ObjectPtr owner);

    //! @brief オーナーを取得
    //! @return オーナーオブジェクト
    ObjectPtr GetOwner() const;

    //! @brief 遷移可能か
    //! @details ダメージを受けた後に前のアクションに戻る場合などに使用
    //! @return 戻る可能ならtrue
    virtual bool CanTransit() const
    {
        return true;
    }

    //! @brief アクションを設定
    //! @param action 設定するアクション
    void SetAction(std::shared_ptr<EnemyActionBase> action);

    //! @brief 現在のアクション取得
    //! @return 現在のアクション
    std::shared_ptr<EnemyActionBase> GetCurrentAction() const
    {
        return now_action_;
    }

    //! @brief 攻撃アクションを設定
    virtual void SetAttack() {}

    //! @brief プレイヤーを追いかけるアクションを設定
    virtual void SetFindPlayer() {}

    //! @brief ノックバックアクションを設定
    virtual void SetKnockback() {}

    //! @brief 衝突時のコールバック
    virtual void OnHit([[maybe_unused]] const ComponentCollision::HitInfo& hit_info) {}

protected:
    //! @brief プレイヤーとの角度を取得
    //! @return プレイヤーとの角度
    float FacePlayerDegree() const;

    //! @brief プレイヤーとの距離を取得
    //! @return プレイヤーとの距離
    float GetPlayerDistance() const;

    //! @brief プレイヤーの方向への角度を取得
    //! @return プレイヤーの方向への角度
    float GetPlayerDegree() const;

    //! @brief プレイヤーを取得
    //! @return プレイヤーオブジェクト
    ObjectPtr GetPlayer() const;

    //! @brief 攻撃コリジョンを追加
    //! @param node アタッチするノード
    //! @param radius 半径
    void AddSphereCollision(std::string_view node, float radius);

    //! @brief 攻撃コリジョンを削除
    void RemoveSphereCollisions();

    ObjectPtr owner_                              = nullptr;   //!< オーナーオブジェクト
    std::shared_ptr<EnemyActionBase> now_action_  = nullptr;   //!< 現在のアクション
    std::shared_ptr<EnemyActionBase> prev_action_ = nullptr;   //!< 前のアクション
    bool attack_hit_                              = false;     //!< 攻撃がヒットしたか(AI採点用)
};

#pragma once
//===========================================================================
//! @file EnemyMove.h
//! @brief 敵移動コンポーネント
//===========================================================================

//---------------------------------------------------------------------------
//! @class EnemyMove
//! @brief 敵移動コンポーネント
//---------------------------------------------------------------------------
class EnemyMove : public Component
{
public:
    BP_COMPONENT_DECL(EnemyMove, u8"敵移動コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 更新後処理
    void PostUpdate() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief ターゲットを設定
    //! @param target ターゲットオブジェクト
    //! @param move_local ローカル座標で移動するか
    void SetTarget(ObjectPtr target, bool move_local = false);

    //! @brief ターゲット座標を設定
    //! @param pos ターゲット座標
    //! @param move_local ローカル座標で移動するか
    void SetPosTarget(float3& pos, bool move_local = false);

    //! @brief 移動速度を設定
    //! @param speed 移動速度
    void SetSpeed(float speed)
    {
        speed_ = speed;
    }

    //! @brief 移動速度を取得
    //! @return 移動速度
    float GetSpeed() const
    {
        return speed_;
    }

    //! @brief 移動ベクトルを設定
    //! @param vel 移動ベクトル
    void SetVelocity(float3 vel);

    //! @brief 移動ベクトルを取得
    //! @return 移動ベクトル
    float3 GetVelocity() const
    {
        return velocity_;
    }

    float GetVelocityLength() const
    {
        float len = dot(velocity_, velocity_);
        if(len < FLT_EPSILON)
        {
            return 0.0f;
        }
        return sqrtf(len);
    }

    //! @brief ターゲットを取得
    //! @return ターゲットオブジェクト
    Object* GetTarget() const
    {
        return target_.lock().get();
    }

    //! @brief ターゲット座標を取得
    //! @return ターゲット座標
    float3 GetTargetPos() const
    {
        auto obj = target_.lock().get();
        if(obj)
        {
            return obj->GetTranslate();
        }
        else
        {
            return target_pos_;
        }
    }

    //! @brief 移動可能フラグを設定
    //! @param can_move 移動可能か
    void SetMove(bool can_move = false)
    {
        can_move_ = can_move;
    }

    //! @brief 移動可能か取得
    //! @return 移動可能か
    bool CanMove() const
    {
        return can_move_;
    }

private:
    std::weak_ptr<Object> target_;   //!< ターゲット
    float3 target_pos_{0, 0, 0};     //!< ターゲット座標
    bool is_set_pos_ = false;        //!< 座標指定か
    float3 velocity_{0, 0, 0};       //!< 移動ベクトル
    float speed_     = 0.5f;         //!< 移動速度
    bool move_local_ = false;        //!< ローカル移動か
    bool can_move_   = true;         //!< 移動可能か
};

#pragma once
//===========================================================================
//! @file BossConfront.h
//! @brief ボスの対峙コンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

USING_PTR(BossConfront);
USING_PTR(BossConfrontFactory);

//---------------------------------------------------------------------------
//! @class BossConfront
//! @brief ボスの対峙コンポーネント
//---------------------------------------------------------------------------
class BossConfront : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(BossConfront, u8"ボスの対峙コンポーネント");

    //! @brief 対峙アクションのタイプ
    enum class ConfrontType
    {
        STAY,         //!< 停止
        LEFT_RIGHT,   //!< 左右移動
        FORWARD,      //!< 前進
        BACKWARD,     //!< 後退

        MAX
    };

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 遅延描画
    void LateDraw() override;

    //! @brief 更新後処理
    void PostUpdate() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 対峙アクションを取得
    //! @param type 取得したいアクションの種類
    //! @return アクションのポインタ
    BossConfrontFactoryPtr GetConfrontAction(ConfrontType type) const
    {
        if(current_confront_type_ != type)
        {
            return nullptr;
        }
        return confront_action_;
    }

    //! @brief 対峙アクションを設定
    //! @param type 設定するアクションの種類
    void SetConfrontAction(ConfrontType type);

    //! @brief ランダムな対峙アクションを設定
    void SetRandomConfrontAction();

protected:
    float3 player_prev_sec_pos_ = float3{0.0f, 0.0f, 0.0f};   //!< 1フレーム前のプレイヤーの位置
    //! @brief プレイヤーが正面にいるか判定
    //! @retval true 正面にいる
    //! @retval false 正面にいない
    bool IsPlayerInFront();

private:
    BossConfrontFactoryPtr confront_action_ = nullptr;              //!< 現在の対峙アクション
    ConfrontType current_confront_type_     = ConfrontType::STAY;   //!< 現在の対峙アクションの種類
    float key_timer_                        = 0.0f;                 //!< キー入力タイマー
    static constexpr float CLOSE_DISTANCE_  = 15.0f;                //!< 後退する距離
    static constexpr int BACKWARD_RATE_     = 30;                   //!< 後退する確率
};

//---------------------------------------------------------------------------
//! @class BossConfrontFactory
//! @brief ボスの対峙アクションの基底クラス
//---------------------------------------------------------------------------
class BossConfrontFactory : public BossConfront
{
public:
    BP_COMPONENT_DECL(BossConfrontFactory, u8"ボスの対峙アクションコンポーネント");

    //! @brief 対峙アクションの初期化
    virtual void ConfrontInit() {}

    //! @brief 対峙アクションの更新
    virtual void ConfrontUpdate() {}

    //! @brief 対峙アクションの終了
    virtual void ConfrontExit() {}

    //! @brief オーナーを設定
    //! @param owner オーナー
    void SetOwner(ObjectPtr owner)
    {
        owner_ = owner;
    }

    //! @brief オーナーを取得
    //! @return オーナー
    ObjectPtr GetOwner() const
    {
        return owner_;
    }

    //! @brief 親コンポーネントを設定
    //! @param parent 親コンポーネント
    void SetParent(const std::shared_ptr<BossConfront>& parent)
    {
        parent_ = parent;
    }

    //! @brief 親コンポーネントを取得
    //! @return 親コンポーネント
    std::shared_ptr<BossConfront> GetParent() const
    {
        return parent_.lock();
    }

private:
    ObjectPtr owner_ = nullptr;            //!< オーナー
    std::weak_ptr<BossConfront> parent_;   //!< 親コンポーネント
};

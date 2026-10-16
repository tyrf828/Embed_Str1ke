#pragma once
//===========================================================================
//! @file UltimateAttack.h
//! @brief 必殺技コンポーネント
//===========================================================================
#include <Game/Component/Player/UltimateAttack/UltimateBase.h>
#include <Game/Object/Player/UltimateAttackCollision.h>

//---------------------------------------------------------------------------
//! @class UltimateAttack
//! @brief 必殺技コンポーネント
//---------------------------------------------------------------------------
class UltimateAttack : public Component
{
public:
    BP_COMPONENT_DECL(UltimateAttack, u8"必殺技コンポーネント");
    //! @brief 初期化
    void Init() override;
    //! @brief 更新
    void Update() override;
    //! @brief 描画
    void LateDraw() override;
    //! @brief GUI表示処理
    void GUI() override;
    //! @brief 終了
    void Exit() override;

    //! @brief アクションを設定
    //! @param action 設定するアクション
    void SetAction(std::shared_ptr<UltimateBase> action)
    {
        if(now_action_)
        {
            now_action_->Exit();
        }
        now_action_ = action;
        if(now_action_)
        {
            now_action_->SetOwner(GetOwnerPtr());
            now_action_->SetParent(this);
            now_action_->Init();
        }
    }

    //! @brief 攻撃範囲を取得
    //! @return 攻撃範囲
    float GetRadius() const
    {
        return ATTACK_RADIUS_;
    }

    //! @brief 攻撃位置を設定
    //! @param attack_pos 攻撃位置
    void SetAttackPos(float3 attack_pos)
    {
        attack_pos_ = attack_pos;
    }

    //! @brief 攻撃位置を取得
    //! @return 攻撃位置
    float3 GetAttackPos() const
    {
        return attack_pos_;
    }

private:
    std::shared_ptr<UltimateBase> now_action_    = nullptr;              //!< 現在のアクション
    UltimateAttackCollisionPtr attack_collision_ = nullptr;              //!< 攻撃の衝突判定
    static constexpr float ATTACK_RADIUS_        = 50.0f;                //!< 攻撃範囲
    float3 attack_pos_                           = {0.0f, 0.0f, 0.0f};   //!< 攻撃位置
};

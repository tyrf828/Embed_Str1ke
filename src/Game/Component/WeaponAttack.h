#pragma once
//===========================================================================
//! @file WeaponAttack.h
//! @brief 武器攻撃の管理
//===========================================================================

//---------------------------------------------------------------------------
//! @class WeaponAttack
//! @brief 武器攻撃の管理
//---------------------------------------------------------------------------
class WeaponAttack : public Component
{
public:
    BP_COMPONENT_DECL(WeaponAttack, u8"武器攻撃コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 更新後処理
    void LateUpdate() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 攻撃判定を消す
    void SetNoHit();

    //! @brief 攻撃判定付ける
    void SetHit();

    //! @brief 攻撃力を設定
    //! @param atk_point 設定する攻撃力
    void SetBaseATK(int atk_point);

    //! @brief 攻撃力を取得
    //! @return 攻撃力
    int GetBaseATK() const;

    //! @brief 最終攻撃力を取得
    //! @return 最終攻撃力
    int GetATK() const;

    //! @brief 敵にヒットした時の処理
    //! @param hit_info ヒット情報
    void HitEnemy(const ComponentCollision::HitInfo& hit_info);

    //! @brief ヒットフィードバック
    //! @param effect_position エフェクト発生位置
    void HitFeedback(const float3& effect_position);

private:
    int attack_point_                       = 0;      //!< 攻撃力
    int final_atk_                          = 0;      //!< 最終攻撃力
    static constexpr float ATTACK_MULTIPLY_ = 1.3f;   //!< 攻撃力倍率
};

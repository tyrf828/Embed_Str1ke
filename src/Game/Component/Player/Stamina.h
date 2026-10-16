#pragma once
//===========================================================================
//! @file Stamina.h
//! @brief プレイヤーのスタミナ
//===========================================================================

//---------------------------------------------------------------------------
//! @class Stamina
//! @brief プレイヤーのスタミナクラス
//---------------------------------------------------------------------------
class Stamina : public Component
{
public:
    BP_COMPONENT_DECL(Stamina, u8"スタミナコンポーネント");

    //! @brief 初期化
    //! @return 初期化済み
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief スタミナを取得
    //! @return スタミナ
    int GetStamina() const;

    //! @brief スタミナを減らす
    //! @param stamina スタミナを減らす設定量
    void SubStamina(int stamina);

    //! @brief 最大スタミナを取得
    //! @return 最大スタミナ
    int GetMaxStamina() const;

    //! @brief スタミナ回復のクールタイムを取得
    //! @return 回復のクールタイム
    float GetStaminaCoolTime() const;

    //! @brief スタミナ回復のクールタイムを最大数に設定
    void SetStaminaCoolTime();

private:
    static constexpr int MAX_STAMINA_             = 1000;           //!< スタミナの最大数
    int stamina_                                  = MAX_STAMINA_;   //!< スタミナ
    static constexpr float STAMINA_COOL_TIME_MAX_ = 1.5f;           //!< 回復のクールタイム
    static constexpr int STAMINA_RECOVER_SPEED_   = 5;              //!< 回復速度
    float stamina_cool_time_                      = 0;              //!< 回復のクールタイム
};

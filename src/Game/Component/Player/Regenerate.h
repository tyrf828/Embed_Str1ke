#pragma once
//===========================================================================
//! @file Regenerate.h
//! @brief プレイヤーの回復コンポーネント
//===========================================================================

//---------------------------------------------------------------------------
//! @class Regenerate
//! @brief 回復コンポーネント
//---------------------------------------------------------------------------
class Regenerate : public Component
{
public:
    BP_COMPONENT_DECL(Regenerate, u8"回復コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 終了
    void Exit() override;

private:
    static constexpr int REGEN_HP_     = 750;     //!< 回復量
    static constexpr float REGEN_TIME_ = 2.55f;   //!< 回復を始める時間
    bool regen_trigger_                = false;   //!< 回復トリガー
};

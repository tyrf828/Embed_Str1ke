#pragma once
//===========================================================================
//! @file UltimateGauge.h
//! @brief プレイヤーの必殺技エネルギーゲージコンポーネント
//===========================================================================

//---------------------------------------------------------------------------
//! @class UltimateGauge
//! @brief 必殺技エネルギーゲージ
//---------------------------------------------------------------------------
class UltimateGauge : public Component
{
public:
    BP_COMPONENT_DECL(UltimateGauge, u8"必殺技エネルギーゲージコンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 終了
    void Exit() override;

    //! @brief エネルギーを追加
    void AddEnergy(int energy);

    //! @brief エネルギーを取得
    //! @return 現在の必殺技エネルギー
    int GetEnergy() const
    {
        return ultimate_energy_;
    }

    //! @brief 必殺技の消費コストを取得
    int GetUltimateCost() const
    {
        return ULTIMATE_COST_;
    }

    //! @brief エネルギーを設定
    //! @param energy 設定するエネルギー値
    void SetEnergy(int energy);

private:
    static constexpr int ULTIMATE_COST_ = 1000;   //!< 必殺技の消費コスト
    int ultimate_energy_                = 0;      //!< 現在の必殺技エネルギー
};

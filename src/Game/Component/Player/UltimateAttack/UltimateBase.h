#pragma once
//===========================================================================
//! @file UltimateBase.h
//! @brief 必殺技ベースクラス
//===========================================================================

USING_PTR(UltimateAttack);

//---------------------------------------------------------------------------
//! @class UltimateBase
//! @brief 必殺技ベースクラス
//---------------------------------------------------------------------------
class UltimateBase : public Component
{
public:
    BP_COMPONENT_DECL(UltimateBase, u8"必殺技ベースクラス");

    //! @brief 初期化
    virtual void Init() {}

    //! @brief 更新
    virtual void Update() {}

    //! @brief 描画
    virtual void LateDraw() {}

    //! @brief 終了
    virtual void Exit() {}

    //! @brief GUI表示処理
    virtual void GUI() {}

    //! @brief オーナーを設定
    //! @param owner オーナーオブジェクト
    void SetOwner(ObjectPtr owner);

    //! @brief オーナーを取得
    //! @return オーナーオブジェクト
    ObjectPtr GetOwner() const;

    //! @brief 親コンポーネントを設定
    //! @param ultimate_attack 親コンポーネント
    void SetParent(UltimateAttack* ultimate_attack);

protected:
    ObjectPtr owner_                 = nullptr;   //!< オーナーオブジェクト
    UltimateAttack* ultimate_attack_ = nullptr;   //!< 必殺技コンポーネント
};

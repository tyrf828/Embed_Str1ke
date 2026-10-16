#pragma once
//===========================================================================
//! @file AttachWeaponRemain.h
//! @brief アタッチ武器使える回数
//===========================================================================

//---------------------------------------------------------------------------
//! @class AttachWeaponRemain
//! @brief アタッチ武器使える回数
//---------------------------------------------------------------------------
class AttachWeaponRemain : public Component
{
public:
    BP_COMPONENT_DECL(AttachWeaponRemain, u8"アタッチ武器使える回数");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 更新後
    void PostUpdate() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 残り回数を取得
    //! @return 残り回数
    int GetRemain() const
    {
        return remain_;
    }

    //! @brief 最大回数を取得
    //! @return 最大回数
    int GetMaxRemain() const
    {
        return max_remain_;
    }

    //! @brief 残り回数を設定
    //! @param value 残り回数
    void SetRemain(int value)
    {
        remain_ = value;
    }

    //! @brief 最大回数を設定
    //! @param remain 最大回数
    //! @param reset_remain 残り回数をリセットするか
    void SetMaxRemain(int remain, bool reset_remain = false)
    {
        max_remain_ = remain;
        if(reset_remain)
        {
            remain_ = max_remain_;
        }
    }

    //! @brief 残り回数を減らす
    //! @param value 減らす値
    void SubRemain(int value)
    {
        remain_ -= value;
    }

private:
    int remain_     = 8;   //!< 残り回数
    int max_remain_ = 8;   //!< 最大回数
};

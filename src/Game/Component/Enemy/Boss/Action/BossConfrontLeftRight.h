#pragma once
//===========================================================================
//! @file BossConfrontLeftRight.h
//! @brief ボスの左右移動コンポーネント
//===========================================================================
#include "BossConfront.h"

//---------------------------------------------------------------------------
//! @class BossConfrontLeftRight
//! @brief ボスの左右移動コンポーネント
//---------------------------------------------------------------------------
class BossConfrontLeftRight : public BossConfrontFactory
{
public:
    BP_COMPONENT_DECL(BossConfrontLeftRight, u8"ボスの左右移動コンポーネント");

    //! @brief 初期化
    void ConfrontInit() override;

    //! @brief 更新
    void ConfrontUpdate() override;

    //! @brief 終了
    void ConfrontExit() override;

    //! @brief 移動方向を設定
    //! @param to_right true:右, false:左
    void SetMoveDirection(bool to_right)
    {
        move_to_right_ = to_right;
    }

    //! @brief プレイヤーが左右どちらにいるか判定
    //! @param check_right true:右側をチェック, false:左側をチェック
    //! @return 指定した方向にいる場合true
    bool IsPlayerInLeftRight(bool check_right);

    //! @brief 右方向ベクトルを取得
    //! @return 右方向ベクトル
    float3 GetRightDirection() const;

private:
    bool move_to_right_ = true;   //!< 右に移動するかどうか
};

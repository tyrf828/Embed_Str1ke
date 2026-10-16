#pragma once
//===========================================================================
//! @file Knockback.h
//! @brief 敵のノックバック
//===========================================================================
#include <Game/Component/ForceMoveBase.h>

//---------------------------------------------------------------------------
//! @class Knockback
//! @brief 敵のノックバック
//---------------------------------------------------------------------------
class Knockback : public ForceMoveBase
{
public:
    BP_COMPONENT_DECL(Knockback, u8"敵のノックバック");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 遅延描画
    void LateDraw() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 終了
    void Exit() override;

    //! @brief 敵側の OnHit から呼ばれる通知
    //! @param hit_info 衝突情報
    void IsHitGround(const ComponentCollision::HitInfo& hit_info);

    //! @brief 方向を設定
    //! @param dir 方向
    //! @param force_power 力の強さ
    void SetDirection(float3 dir, float force_power = 2.0f)
    {
        ForceMoveBase::SetDirection(dir, force_power);
    }

private:
    bool landed_ = false;   //!< 地面に接地したか
};

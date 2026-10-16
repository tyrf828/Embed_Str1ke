#pragma once
//===========================================================================
//! @file AttachAttack.h
//! @brief アタッチ攻撃コンポーネント
//===========================================================================

//---------------------------------------------------------------------------
//! @class AttachAttack
//! @brief アタッチ攻撃コンポーネント
//---------------------------------------------------------------------------
class AttachAttack : public Component
{
public:
    BP_COMPONENT_DECL(AttachAttack, u8"アタッチ攻撃コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 衝突時の処理
    //! @param hit_info 衝突したオブジェクトの情報
    void OnHit(const ComponentCollision::HitInfo& hit_info);

    //! @brief 描画
    void LateDraw() override;

    //! @brief GUI表示処理
    void GUI() override;

private:
};

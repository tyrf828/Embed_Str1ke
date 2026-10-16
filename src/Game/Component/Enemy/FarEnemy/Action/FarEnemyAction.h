#pragma once
//===========================================================================
//! @file FarEnemyAction.h
//! @brief 敵の行動コンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class FarEnemyAction
//! @brief 敵の行動コンポーネント
//---------------------------------------------------------------------------
class FarEnemyAction : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(FarEnemyAction, u8"敵の行動コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 更新後処理
    void PostUpdate() override;

    void LateDraw() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 衝突時の処理
    //! @param hit_info 衝突したオブジェクトの情報
    void OnHit(const ComponentCollision::HitInfo& hit_info);

    //! @brief 前のアクションを取得
    //! @return 前のアクション
    std::shared_ptr<EnemyActionBase> GetPrevAction() const;

    //! @brief 攻撃アクションを設定
    void SetAttack() override;

    //! @brief ノックバックアクションを設定
    void SetKnockback() override;

    //! @brief プレイヤーを探すアクションを設定
    void SetFindPlayer() override;

private:
};

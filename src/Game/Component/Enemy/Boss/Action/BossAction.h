#pragma once
//===========================================================================
//! @file BossAction.h
//! @brief ボスの行動コンポーネント
//===========================================================================
#include <Game/Component/Enemy/EnemyActionBase.h>

//---------------------------------------------------------------------------
//! @class BossAction
//! @brief ボスの行動コンポーネント
//---------------------------------------------------------------------------
class BossAction : public EnemyActionBase
{
public:
    BP_COMPONENT_DECL(BossAction, u8"ボスの行動コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    void LateDraw() override;

    void PostUpdate() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 衝突時の処理
    //! @param hit_info 衝突したオブジェクトの情報
    void OnHit(const ComponentCollision::HitInfo& hit_info);

    //! @brief 前のアクションを取得
    //! @return 前のアクション
    std::shared_ptr<EnemyActionBase> GetPrevAction() const;

    //! @brief ノックバックを設定
    void SetKnockback() override;

private:
};

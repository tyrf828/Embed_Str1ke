#pragma once
//===========================================================================
//! @file StageTrigger.h
//! @brief ステージのトリガー
//===========================================================================

USING_PTR(StageTrigger);
//---------------------------------------------------------------------------
//! @class StageTrigger
//! @brief ステージのトリガー(ステージと交互用)
//---------------------------------------------------------------------------
class StageTrigger : public Object
{
public:
    BP_OBJECT_DECL(StageTrigger, "ステージのトリガー");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 衝突時の処理
    //! @param hit_info 衝突したオブジェクトの情報
    void OnHit(const ComponentCollision::HitInfo& hit_info) override;

    //! @brief 衝突判定
    //! @retval true 衝突している
    //! @retval false 衝突していない
    bool IsHit();

    void SetSize(float3 size);

private:
    bool is_hit_ = false;   //!< 当たったかどうか
};

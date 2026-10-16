#pragma once
//===========================================================================
//! @file CircularBoundary.h
//! @brief 円形境界コンポーネント
//===========================================================================
USING_PTR(CircularBoundary);

//----------------------------------------------------------
//! @class CircularBoundary
//! @brief プレイヤーを指定した半径の円内に閉じ込めるコンポーネント
//----------------------------------------------------------
class CircularBoundary : public Component
{
public:
    BP_COMPONENT_DECL(CircularBoundary, u8"円形境界");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示
    void GUI() override;

    //! @brief 境界の中心座標を設定
    //! @param center 中心座標
    void SetCenter(const float3& center)
    {
        center_ = center;
    }

    //! @brief 境界の半径を設定
    //! @param radius 半径
    void SetRadius(float radius)
    {
        radius_ = radius;
    }

    //! @brief 境界の中心座標を取得
    //! @return 中心座標
    float3 GetCenter() const
    {
        return center_;
    }

    //! @brief 境界の半径を取得
    //! @return 半径
    float GetRadius() const
    {
        return radius_;
    }

    //! @brief 境界対象オブジェクトを追加
    //! @param object オブジェクト
    //! @return 自分自身のshared_ptr
    CircularBoundaryPtr AddBoundaryObject(const ObjectWeakPtr object)
    {
        objects_.push_back(object);
        return std::dynamic_pointer_cast<CircularBoundary>(shared_from_this());
    }

    //! @brief 境界対象オブジェクトをクリア
    void ClearBoundaryObjects()
    {
        objects_.clear();
    }

private:
    float3 center_ = {0.0f, 0.0f, 0.0f};   //!< 境界の中心座標
    float radius_  = 125.0f;               //!< 境界の半径
    ObjectWeakPtrVec objects_;             //!< 制限対象オブジェクト
};

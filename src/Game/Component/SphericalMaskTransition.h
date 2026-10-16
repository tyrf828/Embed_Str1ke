#pragma once
//===========================================================================
//! @file SphericalMaskTransition.h
//! @brief 球面トランジション
//===========================================================================

USING_PTR(SphericalMaskTransition);

//---------------------------------------------------------------------------
//! @class SphericalMaskTransition
//! @brief 球面トランジション
//---------------------------------------------------------------------------
class SphericalMaskTransition : public Component
{
public:
    BP_COMPONENT_DECL(SphericalMaskTransition, u8"SphericalMaskTransition");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 更新後処理
    void PostUpdate() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI描画
    void GUI() override;

    //! @brief マスクのワールド座標を設定
    //! @param pos ワールド座標
    //! @return 自身のポインタ
    SphericalMaskTransitionPtr SetMaskWorldPosition(const float3& pos)
    {
        mask_world_position_ = pos;
        return std::dynamic_pointer_cast<SphericalMaskTransition>(shared_from_this());
    }

    //! @brief マスクの半径を設定
    //! @param radius 半径
    //! @return 自身のポインタ
    SphericalMaskTransitionPtr SetMaskRadius(float radius)
    {
        mask_radius_ = radius;
        return std::dynamic_pointer_cast<SphericalMaskTransition>(shared_from_this());
    }

    //! @brief マスクの境界の柔らかさを設定
    //! @param softness 柔らかさ
    //! @return 自身のポインタ
    SphericalMaskTransitionPtr SetMaskSoftness(float softness)
    {
        mask_softness_ = softness;
        return std::dynamic_pointer_cast<SphericalMaskTransition>(shared_from_this());
    }

    //! @brief マスクの反転設定
    //! @param inverse 反転設定 true:内側を非表示, false:外側を非表示
    //! @return 自身のポインタ
    SphericalMaskTransitionPtr SetMaskInverse(bool inverse)
    {
        mask_inverse_ = inverse;
        return std::dynamic_pointer_cast<SphericalMaskTransition>(shared_from_this());
    }

    //! @brief トランジション開始
    //! @return 自身のポインタ
    SphericalMaskTransitionPtr StartTransition();

    //! @brief 削除制限を設定
    //! @param limit 制限値
    //! @param is_destroy_object オブジェクトを削除するか
    //! @return 自身のポインタ
    SphericalMaskTransitionPtr SetDestroyLimit(float limit, bool is_destroy_object = false)
    {
        destroy_limit_            = limit;
        destory_after_transition_ = is_destroy_object;
        return std::dynamic_pointer_cast<SphericalMaskTransition>(shared_from_this());
    }

    //! @brief 速度を設定
    //! @param speed 速度
    //! @return 自身のポインタ
    SphericalMaskTransitionPtr SetSpeed(float speed)
    {
        speed_ = speed;
        return std::dynamic_pointer_cast<SphericalMaskTransition>(shared_from_this());
    }

private:
    std::shared_ptr<ShaderPs> shader_ps_inner_;   //!< ピクセルシェーダー(内側表示)
    std::shared_ptr<ShaderPs> shader_ps_outer_;   //!< ピクセルシェーダー(外側表示)
    bool last_inverse_         = true;            //!< 前の状態
    inline static float speed_ = 3.0f;            //!< トランジション速度

    inline static float3 mask_world_position_ = float3{0.0f, 0.0f, 0.0f};   //!< マスク球の中心
    inline static float mask_radius_          = 0.0f;                       //!< マスク球の半径
    inline static float mask_softness_        = 5.0f;                       //!< 境界の柔らかさ
    inline static int instance_count_         = 0;                          //!< インスタンス数
    bool mask_inverse_                        = false;                      //!< マスク反転フラグ
    struct SpericalInfo
    {
        float4 mask_world_position_;   //!< マスク球の中心

        float mask_radius_;     //!< マスク球の半径
        float mask_softness_;   //!< 境界の柔らかさ
    };
    int spherical_info_            = -1;      //!< 定数バッファ
    bool start_transition_         = false;   //!< トランジション開始フラグ
    float destroy_limit_           = 0.0f;    //!< 削除制限
    bool destory_after_transition_ = false;   //!< トランジション終了後にオブジェクトを削除するか
};

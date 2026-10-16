#pragma once
//===========================================================================
//! @file ComponentCameraController.h
//! @brief カメラ制御コンポーネント
//===========================================================================

USING_PTR(ComponentCameraController);

//---------------------------------------------------------------------------
//! @class ComponentCameraController
//! @brief カメラ制御コンポーネント
//---------------------------------------------------------------------------
class ComponentCameraController : public Component
{
public:
    BP_COMPONENT_DECL(ComponentCameraController, u8"カメラコンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief ターゲットを設定
    //! @param target カメラのターゲット
    ComponentCameraControllerPtr SetTarget(ObjectPtr target);

    //! @brief カメラの回転角度を設定
    //! @param rot 設定する回転角度
    ComponentCameraControllerPtr SetCameraRotate(float3 rot);

    //! @brief カメラの回転制限を設定
    //! @param limit_cam_up カメラの上方向の制限角度
    //! @param limit_cam_down カメラの下方向の制限角度
    ComponentCameraControllerPtr SetCameraRotateLimit(float limit_cam_up, float limit_cam_down);

    //! @brief 操作を設定
    //! @param use 操作するか
    ComponentCameraControllerPtr SetUseInput(bool use = true);

    //! @brief 操作できるか
    //! @retval true できる
    //! @retval false できない
    bool IsUseInput() const;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief カメラをプレイヤーの背後に設定
    void SetCameraToPlayerBehind();

    //! @brief カメラの回転軸を取得
    //! @return カメラの回転軸
    float2 GetCameraRotateAxis() const;

    //! @brief カメラの回転軸を設定
    //! @param rx カメラの回転軸
    void SetCameraRotateAxis(float2 rx);

    //! @brief カメラの回転軸を加算
    //! @param rx 加算するカメラの回転軸
    void AddCameraRotateX(float rx);

private:
    float cam_rx_ = 0;   //!< カメラのX軸回転角度
    float cam_ry_ = 0;   //!< カメラのY軸回転角度

    bool use_input_         = false;   //!< 操作するか
    float mouse_up_down_    = 10.0f;   //!< マウスによる上下移動の感度
    float mouse_left_right_ = 10.0f;   //!< マウスによる左右移動の感度

    float limit_cam_up_   = 89.0f;    //!< カメラの上方向回転制限
    float limit_cam_down_ = -89.0f;   //!< カメラの下方向回転制限

    ObjectWeakPtr target_;                 //!< カメラのターゲット
    float target_cam_side_speed_ = 3.0f;   //!< ロックオン時のカーソル移動速度
};

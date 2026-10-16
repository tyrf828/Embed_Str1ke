#pragma once
//===========================================================================
//! @file CameraAimPoint.h
//! @brief カメラが見る場所
//===========================================================================

USING_PTR(CameraAimPoint);

//---------------------------------------------------------------------------
//! @class CameraAimPoint
//! @brief カメラが見る場所
//---------------------------------------------------------------------------
class CameraAimPoint : public Object
{
public:
    BP_OBJECT_DECL(CameraAimPoint, "CameraAimPoint");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief プレイヤーにバインドするか
    //! @retval true バインドする
    //! @retval false バインドしない
    void SetBindPlayer(bool bind = true);

private:
    static constexpr float CAMERA_MOVE_SPEED_ = 0.2f;                  //!< カメラの移動速度 (0.0f ~ 1.0f)
    bool bind_player_                         = true;                  //!< プレイヤーにバインドするか
    float3 player_pos_                        = {0.0f, 0.0f, 0.0f};    //!< プレイヤーの位置
    float3 offset_                            = {0.0f, 23.0f, 0.0f};   //!< プレイヤーの頭上オフセット
};

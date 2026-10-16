//----------------------------------------------------------------------------
//!	@file	camera.h.fx
//!	@brief	カメラ関連
//----------------------------------------------------------------------------
#ifndef CAMERA_H_FX
#define CAMERA_H_FX

cbuffer CameraInfo : register(b10)
{
    matrix mat_view_;           //!< ビュー行列
    matrix mat_proj_;           //!< 投影行列
    matrix mat_inv_view_;       //!< ビュー行列の逆行列
    matrix mat_inv_proj_;       //!< 投影行列の逆行列
    float3 eye_position_;       //!< カメラの位置
    float padding0_;
    float2 resolution_;         //!< 画面解像度
    
    // シャドウ用
    matrix light_view_proj_[4]; //!< ライトのビュー投影行列
    
    float delta_time_;
};

#endif // CAMERA_H_FX


#pragma once
//===========================================================================
//! @file Camera.h
//! @brief カメラ
//===========================================================================
#include <Game/Object/Enemy/EnemyLockPoint.h>

USING_PTR(Camera);

//---------------------------------------------------------------------------
//! @class Camera
//! @brief カメラ
//---------------------------------------------------------------------------
class Camera : public Object
{
public:
    BP_OBJECT_DECL(Camera, "Camera");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 衝突時の処理
    //! @param hit_info 衝突したオブジェクトの情報
    void OnHit(const ComponentCollision::HitInfo& hit_info) override;

    //! @brief 更新後
    void PostUpdate() override;

    //! @brief 描画
    void LateDraw() override;

    //! @brief 終了
    void Exit() override;

    //! @brief カメラの揺れ処理
    void ShakeCamera(float sec, float power);

    //! @brief カメラの前方向ベクトルを取得
    //! @return カメラの前方ベクトル
    float3 GetFront() const;

    //! @brief カメラの平面方向ベクトルを取得
    //! @return カメラの平面上の前方ベクトル
    float3 GetPlaneFront() const;

    //! @brief カメラの右方向ベクトルを取得
    //! @return カメラの右方向ベクトル
    float3 GetRight() const;

    //! @brief カメラの平面右方向ベクトルを取得
    //! @return カメラの平面上の右方向ベクトル
    float3 GetPlaneRight() const;

    //! @brief カメラがロックしているか
    //! @retval true している
    //! @retval false していない
    bool IsCameraLock() const;

    //! @brief ロックしている敵を取得
    //! @return ロックしている敵
    EnemyLockPointPtr GetLockTarget() const
    {
        return target_.lock();
    }

    //! @brief カメラの視野角を取得
    //! @return カメラの視野角
    float GetFOV() const
    {
        return FOV;
    }

    //! @brief カメラに映っているか
    //! @retval true 映っている
    //! @retval false 映っていない
    bool IsInCameraView(const float3& position) const;

    //! @brief ロック対象リセット
    void ResetLockTarget();

private:
    static constexpr float SPRING_ARM_MAX_LENGTH_ = 30.0f;   //!< スプリングアームの最大長さ（カメラとプレイヤーの距離に関連）
    float camera_shake_power_                     = 0;       //!< カメラの揺れパワー
    float camera_shake_time_                      = 0.0f;    //!< カメラの揺れ時間

    static constexpr float FOV                   = 68.0f;   //!< カメラの視野角
    float spring_arm_length_                     = 0.0f;    //!< スプリングアームの長さ
    bool is_hit_stage_                           = false;   //!< ステージに当たったか
    static constexpr float HIT_RECOVER_TIME_MAX_ = 0.2f;    //!< ステージから離れた後の回復時間の最大値
    float hit_recover_time_                      = 0.0f;    //!< ステージから離れた後の回復時間
    float distance_                              = 0.0f;    //!< カメラの距離

    float deg_                                  = 0.0f;     //!< ロックできるの角度
    float3 forward_                             = {0.0f};   //!< 前方向ベクトル
    float1 dot_                                 = {0.0f};   //!< ロックできるの角度を計算するため内積
    static constexpr float LOCK_LIMIT_DISTANCE_ = 200.0f;   //!< ロックオンの最大距離
    static constexpr float LOCK_DEGREE_         = 90.0f;    //!< ロックできる角度

    EnemyLockPointWeakPtr target_;      //!< ロック対象
    EnemyBaseWeakPtr lock_candidate_;   //!< ボタンを押すとロックする候補
    int lock_candidate_image_ = -1;     //!< ロック候補の画像

    //! @brief カメラの揺れ処理
    void ShakeCamera();
};

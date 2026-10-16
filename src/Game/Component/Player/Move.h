#pragma once
//===========================================================================
//! @file Move.h
//! @brief プレイヤーの移動コンポーネント
//===========================================================================

USING_PTR(Move);

//---------------------------------------------------------------------------
//! @class Move
//! @brief 移動コンポーネント
//---------------------------------------------------------------------------
class Move : public Component
{
public:
    BP_COMPONENT_DECL(Move, u8"移動コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 移動の有効か無効を設定
    //! @param enable 移動可能か
    void SetEnable(bool enable);

    //! @brief 移動速度を設定
    //! @param speed 移動速度
    void SetMoveSpeed(const float speed);

    //! @brief 回転速度を設定
    //! @param speed 回転速度
    void SetRotateSpeed(const float speed);

    //! @brief 移動キーを設定
    //! @param up    前
    //! @param down  後
    //! @param left  左
    //! @param right 右
    void SetKeys(int up, int down, int left, int right);

    //! @brief 移動速度を取得
    //! @return 移動速度
    const float GetMoveSpeed() const;

    //! @brief 回転速度を取得
    //! @return 回転速度
    const float GetRotateSpeed() const;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 歩きの方向を取得(4方向)
    //! @return 歩き方向(4方向)
    int GetWalkStatus4_Dir() const;

    //! @brief 移動方向を取得
    //! @return 移動方向のベクトル
    float3 GetDir() const;

    float3 GetVelDir() const;

    //----------------------------------------------------------
    //! @enum WALK_STATUS
    //! @brief 歩き方向(4方向)
    //----------------------------------------------------------
    enum WALK_STATUS
    {
        DEFAULT,   //!< デフォルト
        FRONT,     //!< 前
        BACK,      //!< 後
        RIGHT,     //!< 右
        LEFT,      //!< 左
    };

    //----------------------------------------------------------
    //! @enum WALK_KEY_STATUS
    //! @brief キー
    //----------------------------------------------------------
    enum WALK_KEY_STATUS
    {
        KEY_FRONT,   //!< 前
        KEY_BACK,    //!< 後
        KEY_RIGHT,   //!< 右
        KEY_LEFT,    //!< 左

        KEY_MAX,   //!< 最大数
    };

    //! @brief キーが押されているかを取得する
    //! @param key 取得したいキー
    //! @retval true 押されている
    //! @retval false 押されていない
    bool GetKey(WALK_KEY_STATUS key);

    //! @brief 移動中かどうか
    //! @retval true 移動中
    //! @retval false 停止中
    bool IsMoving();

    //! @brief プレイヤーの入力設定
    //! @param front 前
    //! @param back 後
    //! @param right 右
    //! @param left 左
    void SetInput(bool front = false, bool back = false, bool right = false, bool left = false);

private:
    float move_speed_ = 1.0f;   //!< 最大移動速度
    float rot_speed_  = 5.0f;   //!< 回転速度

    float3 velocity_         = {0, 0, 0};   //!< 現在の速度
    float acceleration_time_ = 2.0f;        //!< 加速度
    float damping_time_      = 6.0f;        //!< 減速度
    float speed_ratio_       = 0.0f;        //!< 現在速度の割合

    float3 dir_      = {0, 0, 0};    //!< 今の入力方向
    float3 last_dir_ = {0, 0, -1};   //!< 最後の向き

    int key_up_    = KEY_INPUT_W;   //!< 前
    int key_down_  = KEY_INPUT_S;   //!< 後
    int key_left_  = KEY_INPUT_A;   //!< 左
    int key_right_ = KEY_INPUT_D;   //!< 右

    bool enabled_ = true;   //!< 移動を有効にするか

    int walk_status_                           = 0;     //!< 移動方向(4方向)
    bool key_status_[WALK_KEY_STATUS::KEY_MAX] = {0};   //!< キーの状態
};

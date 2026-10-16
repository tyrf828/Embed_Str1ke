#pragma once
//===========================================================================
//! @file AttachWeapon.h
//! @brief 武器物件にアタッチ
//===========================================================================

//---------------------------------------------------------------------------
//! @class AttachWeapon
//! @brief 武器物件にアタッチ
//---------------------------------------------------------------------------
class AttachWeapon : public Component
{
public:
    BP_COMPONENT_DECL(AttachWeapon, u8"武器物件にアタッチ");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 終了
    void Exit() override;

    //! @brief 武器を設定
    //! @detail 自分を武器オブジェクトにアタッチします
    //! @param position 位置
    //! @param weapon_ptr 武器オブジェクト
    void SetWeapon(float3 position, ObjectPtr weapon_ptr);

    //! @brief 連結可能なフレームがあるか
    //! @param position 位置
    //! @param weapon_ptr 武器オブジェクト(自分)
    //! @param object_ptr オブジェクト(相手)
    //! @return 連結可能か
    static bool CanAttach(float3 position, ObjectPtr weapon_ptr, ObjectPtr object_ptr);

    //! @brief ヒット位置を設定
    //! @param hit_pos ヒット位置
    void SetHitPosition(float3 hit_pos);

    //! @brief 武器関節を取得
    //! @return 武器関節
    ObjectPtr GetWeaponJoint()
    {
        return weapon_joint_;
    }

private:
    //! @brief モデルローカル座標ベースの最も近いフレーム探索
    //! @param model_ptr モデルコンポーネント
    //! @param model_handle モデルハンドル
    //! @param hit_pos ヒット位置
    //! @return 使用中以外の最も近いフレームインデックス
    static int GetNearestFrame(ComponentModelPtr model_ptr, int model_handle, float3 hit_pos);

    ObjectPtr weapon_joint_  = nullptr;              //!< 武器関節
    float3 offset_           = {0.0f, 0.0f, 0.0f};   //!< オフセット
    ObjectPtr weapon_object_ = nullptr;              //!< 武器オブジェクト
    float3 weapon_frame_pos_ = {0.0f, 0.0f, 0.0f};   //!< 武器の位置
    int weapon_frame_index_  = 0;                    //!< 武器フレームインデックス
    float3 weapon_hit_pos_   = {0.0f, 0.0f, 0.0f};   //!< 武器のヒット位置
};

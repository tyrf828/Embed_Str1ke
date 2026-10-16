#pragma once
//===========================================================================
//! @file ScanWave.h
//! @brief スキャン波
//===========================================================================

class AttachWeaponBase;

//---------------------------------------------------------------------------
//! @class ScanWave
//! @brief スキャン波
//---------------------------------------------------------------------------
class ScanWave : public Object
{
public:
    BP_OBJECT_DECL(ScanWave, u8"Scan Wave");

    //! @brief 初期化
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 解放
    void Exit() override;

    //! @brief GUI
    void GUI() override;

    //! @brief スキャン開始
    //! @param position プレイヤーの座標
    void Start(const float3& position);

private:
    //! @brief 隠れた武器を描画
    void DrawOccludedWeapons();

    //! @brief スキャン対象か
    //! @param weapon 武器
    bool CanScan(const std::shared_ptr<AttachWeaponBase>& weapon);

    struct ScanInfo
    {
        float3 world_position_;   //!< 座標
        float3 color_;            //!< 色
        float radius_;            //!< 半径
        float width_;             //!< 幅
        float intensity_;         //!< 光の強さ
        float padding_;           //!< パディング
    };

    struct ColorInfo
    {
        float4 color_;   //!< 輪郭色
    };

    ScanInfo info_;
    float speed_             = 150.0f;                          //!< 速度
    float max_radius_        = 300.0f;                          //!< 最大半径
    float highlight_seconds_ = 5.0f;                            //!< 表示時間
    bool active_             = false;                           //!< 起動中か
    std::vector<std::weak_ptr<AttachWeaponBase>> weapon_list;   //!< 武器リスト
    std::shared_ptr<Texture> texture_;                          //!< 描画用テクスチャ
    std::shared_ptr<Texture> depth_;                            //!< デプス
    std::shared_ptr<Texture> weapon_mask_;                      //!< 武器の表示色と不透明度
    std::shared_ptr<Texture> weapon_depth_;                     //!< 武器デプス
    std::shared_ptr<ShaderPs> shader_;
    std::shared_ptr<ShaderPs> weapon_shader_;
    std::shared_ptr<ShaderPs> composite_shader_;
    int buffer_cb_ = -1;   //!< 定数バッファ
    int weapon_cb_ = -1;   //!< 定数バッファ
};

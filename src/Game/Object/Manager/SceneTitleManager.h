#pragma once
//===========================================================================
//! @file SceneTitleManager.h
//! @brief タイトルシーンマネージャー
//===========================================================================

USING_PTR(SceneTitleManager);
USING_PTR(Bridge);
USING_PTR(Truck);
USING_PTR(LineTransition);
USING_PTR(Rose2D);

//---------------------------------------------------------------------------
//! @class SceneTitleManager
//! @brief タイトルシーンマネージャー
//---------------------------------------------------------------------------
class SceneTitleManager : public Object
{
public:
    BP_OBJECT_DECL(SceneTitleManager, "SceneTitleManager");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 遅延描画
    void LateDraw() override;

    //! @brief 終了
    void Exit() override;

private:
    static constexpr int BRIDGE_MAX_           = 2;                    //!< 橋の最大数
    std::array<BridgePtr, BRIDGE_MAX_> bridge_ = {nullptr, nullptr};   //!< 橋
    ObjectPtr camera_                          = nullptr;              //!< カメラ
    TruckPtr truck_                            = nullptr;              //!< トラック
    ObjectPtr player_                          = nullptr;              //!< プレイヤー
    static constexpr float CAMERA_OFFSET_Y_    = 22.5f;                //!< カメラのYオフセット
    int font_handle_                           = -1;                   //!< フォントハンドル
    int title_ghandle_                         = -1;                   //!< タイトル画像ハンドル
    int text_back_ghandle_                     = -1;                   //!< 文字背景画像ハンドル
    static constexpr float SCENE_MOVE_SPEED_   = 1.5f;                 //!< シーン移動速度

    static constexpr int SELECT_MAX_                  = 3;                              //!< 選択肢の最大数
    int now_select_                                   = 0;                              //!< 現在の選択肢
    std::array<std::string, SELECT_MAX_> select_text_ = {"Start", "Setting", "Exit"};   //!< 選択肢のテキスト

    std::array<std::function<void()>, SELECT_MAX_> select_functions_;   //!< 選択肢に対応する機能
    std::array<float, SELECT_MAX_> select_offset_x_ = {0.0f};           //!< 各選択肢のX軸オフセット
    static constexpr float SELECT_MOVE_SPEED_       = 5.0f;             //!< 選択時の移動速度
    static constexpr float SELECT_MAX_OFFSET_       = 20.0f;            //!< 選択時の最大移動距離
    std::array<int, 2> select_button_ghandle_{};                        //!< 選択ボタン画像ハンドル
    int select_font_handle_ = -1;                                       //!< 選択フォントハンドル

    bool is_start_       = false;   //!< スタートしたかどうか
    bool truck_speed_up_ = false;   //!< トラックの速度が上がったかどうか

    LineTransitionPtr line_transition_ = nullptr;   //!< ライントランジション
    Rose2DPtr rose_effect_             = nullptr;   //!< バラエフェクト

    void BridgeUpdate();            //!< 橋の更新
    void CameraUpdate();            //!< カメラの更新
    void PlayerUpdate();            //!< プレイヤーの更新
    void InitSelectFunctions();     //!< 選択肢の機能を初期化
    void ExecuteSelectFunction();   //!< 選択された機能を実行

public:
    //! @brief 橋を設定
    //! @param bridge 橋
    //! @return 自身のポインタ
    SceneTitleManagerPtr SetBridge(std::array<BridgePtr, BRIDGE_MAX_> bridge)
    {
        bridge_ = bridge;
        return std::dynamic_pointer_cast<SceneTitleManager>(shared_from_this());
    }
    //! @brief カメラを設定
    //! @param camera カメラ
    //! @return 自身のポインタ
    SceneTitleManagerPtr SetCamera(ObjectPtr camera)
    {
        camera_ = camera;
        return std::dynamic_pointer_cast<SceneTitleManager>(shared_from_this());
    }
    //! @brief トラックを設定
    //! @param truck トラック
    //! @return 自身のポインタ
    SceneTitleManagerPtr SetTruck(TruckPtr truck)
    {
        truck_ = truck;
        return std::dynamic_pointer_cast<SceneTitleManager>(shared_from_this());
    }
    //! @brief プレイヤーを設定
    //! @param player プレイヤー
    //! @return 自身のポインタ
    SceneTitleManagerPtr SetPlayer(ObjectPtr player)
    {
        player_ = player;
        return std::dynamic_pointer_cast<SceneTitleManager>(shared_from_this());
    }
    //! @brief ライントランジションを設定
    //! @param line_transition ライントランジション
    //! @return 自身のポインタ
    SceneTitleManagerPtr SetLineTransition(LineTransitionPtr line_transition)
    {
        line_transition_ = line_transition;
        return std::dynamic_pointer_cast<SceneTitleManager>(shared_from_this());
    }
};

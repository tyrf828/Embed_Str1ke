//===========================================================================
//! @file MentalMapBuilder.h
//! @brief メンタルマップビルダー
//===========================================================================
#pragma once

//---------------------------------------------------------------------------
//! @class MentalMapBuilder
//! @brief メンタルマップビルダー
//---------------------------------------------------------------------------
class MentalMapBuilder : public Object
{
public:
    BP_OBJECT_DECL(MentalMapBuilder, u8"メンタルマップビルダー");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 遅延描画
    void LateDraw() override;

    //! @brief GUI
    void GUI() override;

    //! @brief 終了
    void Exit() override;

    //! @brief メンタルマップポイント追加
    //! @param pos 追加位置
    void AddMentalMapPoint(const float3& pos)
    {
        data_.mental_map_points_.push_back(pos);
    }

    //! @brief ナビメッシュターゲット設定
    //! @param target ターゲット位置
    void SetNavmeshTarget(const float3 target)
    {
        navmesh_target_    = target;
        navmesh_target_.y += 1.0f;
    }

    // セーブ用データ構造体
    struct SaveData
    {
        float3 start_pos_;                        //!< マップ開始位置
        float3 end_pos_;                          //!< マップ終了位置
        std::vector<float3> mental_map_points_;   //!< メンタルマップポイント
    };

    //! @brief 領域可視性データ構造体
    struct AreaPointData
    {
        int point_index_;      //!< 点
        int area_index_;       //!< 自分の領域
        int target_area_;      //!< 隣接領域
        int target_sub_bit_;   //!< サブ領域のビットフラグ(1:左下, 2:右下, 4:左上, 8:右上)(3D回転(0.0,0.0,0.0)の場合)
    };

    //! @brief 四分木
    //! @param center 現在の領域の中心
    //! @param size 現在の領域のサイズ
    //! @param points この領域に含まれる点
    //! @param current_depth 現在の階層
    //! @param max_depth 最大階層
    //! @param out_points 出力先
    static void Quadtree(
        float2 center, float2 size, const std::vector<float3>& points, int current_depth, int max_depth, std::vector<std::vector<float3>>& out_points);

    //! @brief マップ読み込み
    static void LoadMap(std::string file_path, SaveData& data);

    //! @brief 領域可視性マップ読み込み
    static void LoadAreaPointMap(std::string file_path, std::vector<AreaPointData>& data, int& split_time);

private:
    //! @brief マップ生成
    void GenerateMap();
    //! @brief ナビメッシュ適用
    void ApplyNavmesh();
    //! @brief マップ保存
    void SaveMap();

    //! @brief 隣接領域の可視性ビルド
    void BuildAreaPointMap();

    // !@brief 隣接領域の可視性保存
    void SaveAreaPointMap();

    bool use_debug_        = true;                                  //!< デバッグ表示フラグ
    std::string file_path_ = "data/Game/MentalMap/MentalMap.txt";   //!< ファイルパス

    SaveData data_;   //!< セーブ用データ

    int num_               = 10;                         //!< 生成数
    bool is_generating_    = false;                      //!< 生成中か
    int now_generate_row_  = 0;                          //!< 現在生成中の行
    int now_generate_col_  = 0;                          //!< 現在生成中の列
    float3 navmesh_target_ = float3{0.0f, 0.0f, 0.0f};   //!< ナビメッシュターゲット位置
    std::string debug_text_;                             //!< デバッグテキスト

    std::string visibility_file_path_ = "data/Game/MentalMap/AreaPointMap.txt";   //!< 可視性ファイルパス

    std::vector<std::vector<float3>> split_area_points_;   //!< 分割された領域の点リスト
    std::vector<AreaPointData> area_point_data_;           //!< 可視性データ
    bool is_building_visibility_ = false;                  //!< 可視性ビルド中か
    int current_build_area_      = 0;                      //!< 現在ビルド中の領域
    int current_build_point_     = 0;                      //!< 現在ビルド中の点

    int built_split_time_ = -1;   //!< 可視性データをビルドしたときの分割回数

    int check_area_ = 0;   //!< チェックする領域
};

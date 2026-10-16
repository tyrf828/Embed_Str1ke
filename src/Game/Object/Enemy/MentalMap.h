//===========================================================================
//! @file MentalMap.h
//! @brief メンタルマップ
//===========================================================================
#pragma once
#include <Game/Object/Tool/MentalMapBuilder.h>
#include <map>

USING_PTR(MentalMap);

//---------------------------------------------------------------------------
//! @class MentalMap
//! @brief メンタルマップ
//---------------------------------------------------------------------------
class MentalMap : public Object
{
public:
    BP_OBJECT_DECL(MentalMap, u8"メンタルマップ");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 遅延
    void LateDraw() override;

    //! @brief GUI
    void GUI() override;

    //! @brief 終了
    void Exit() override;

    //! @brief プレイヤー位置設定
    //! @param pos プレイヤー位置
    void SetPlayerPos(const float3& pos)
    {
        player_pos_ = pos;
        has_target_ = true;
    }

    //! @brief 領域のバウンディングボックスを取得
    const std::vector<std::pair<float3, float3>>& GetMentalBounding() const
    {
        return mental_bounding_;
    }

    //! @brief 分割された領域の点を取得
    const std::vector<std::vector<float3>>& GetSplitAreaPoints() const
    {
        return split_area_points_;
    }

    //! @brief マップサイズを取得
    float2 GetMapSize() const
    {
        return map_size_;
    }

    //! @brief 分割領域の中心を取得
    float2 GetCenter() const
    {
        return center_;
    }

    //! @brief 分割領域の範囲を取得
    float GetSplitAreaRange() const
    {
        return split_area_range_;
    }

    //! @brief 分割回数を取得
    //! @return 分割回数
    int GetSplitTime() const
    {
        return split_time_;
    }

    //! @brief 分割回数を設定
    //! @param split_time 分割回数
    void SetSplitTime(int split_time)
    {
        split_time_ = split_time;
        InitData();
    }

    //! @brief 指定された点が属する領域を取得
    //! @param position ターゲットの位置
    std::vector<int> GetAdjacentAreas(const float3& position);

    //! @brief 指定された領域に隣接する領域を取得
    //! @param area_index ターゲット領域のインデックス
    std::vector<int> GetAdjacentAreas(const int& area_index);

    //! @brief 指定された点から見える領域データを取得
    //! @param area_index 現在の領域
    //! @param point_index 現在の点
    //! @return 可視領域データのリスト
    std::vector<MentalMapBuilder::AreaPointData> GetVisibleData(int area_index, int point_index);

    //! @brief 指定された領域のサブ領域の点を取得
    //! @param area_index ターゲット領域
    //! @param sub_bit サブ領域のビットフラグ(1:左下, 2:右下, 4:左上, 8:右上)
    //! @return サブ領域に含まれる点のリスト
    std::vector<float3> GetSubAreaPoints(int area_index, int sub_bit);

    //! @brief メンタルマップゾーン情報
    struct ZoneData
    {
        int area_index_ = 0;           //<! 領域
        int sub_bit_    = 0;           //!< サブ領域ビットフラグ
        std::vector<float3> points_;   //!< 領域内の点
        int object_     = 0;           //!< オブジェクト数
        int max_object_ = 0;           //!< 最大オブジェクト数
    };

    struct MentalZoneInfo
    {
        ZoneData zone0_;                //!< Zone0 (現在の領域)
        std::vector<ZoneData> zone1_;   //!< Zone1 (可視サブ領域)
        std::vector<ZoneData> zone2_;   //!< Zone2 (不可視サブ領域)
    };

    //! @brief ゾーン情報を取得
    //! @param area_index 現在の領域
    //! @param point_index 現在の点
    //! @return ゾーン情報
    MentalZoneInfo GetMentalZoneInfo(int area_index, int point_index);

    //! @brief ゾーン情報を取得
    //! @return ゾーン情報
    MentalZoneInfo GetMentalZoneInfo();

    //! @brief 敵のターゲット位置を取得
    //! @param info ゾーン情報
    //! @param my_pos 自分の位置
    //! @param excluded_positions 除外する位置のリスト
    //! @param prevent_radius プレイヤーとの距離がこの半径以内の点は除外
    //! @param zone0_search_angle Zone0での検索角度
    //! @param zone1_search_angle Zone1での検索角度
    float3 GetEnemyTargetPos(MentalMap::MentalZoneInfo& info,
                             const float3& my_pos,
                             const std::vector<float3>& excluded_positions = {},
                             const float prevent_radius                    = 15.0f,
                             const float zone0_search_angle                = 30.0f,
                             const float zone1_search_angle                = 60.0f);

    //! @brief 敵の最も近いターゲット位置を取得
    //! @param info ゾーン情報
    //! @param my_pos 自分の位置
    //! @param excluded_positions 除外する位置のリスト
    //! @param prevent_radius プレイヤーとの距離がこの半径以内の点は除外
    //! @param zone0_search_angle Zone0での検索角度
    //! @param zone1_search_angle Zone1での検索角度
    float3 GetEnemyNearestTargetPos(MentalMap::MentalZoneInfo& info,
                                    const float3& my_pos,
                                    const std::vector<float3>& excluded_positions = {},
                                    const float prevent_radius                    = 15.0f,
                                    const float zone0_search_angle                = 30.0f,
                                    const float zone1_search_angle                = 60.0f);

    //! @brief 点の位置が敵が入れるかどうかをチェック
    //! @param point_pos チェックする点の位置
    //! @param enemy_pos 敵の位置
    //! @param prevent_radius プレイヤーとの距離がこの半径以内の点は除外
    //! @param search_angle 検索角度
    //! @param check_behind 敵の背後もチェックするかどうか
    bool CheckPointEntryEnable(
        const float3& point_pos, const float3& enemy_pos, const float prevent_radius = 15.0f, const float search_angle = 30.0f, bool check_behind = false);

    //! @brief ゾーン内のオブジェクト数をクリア
    //! @param info クリアしたいゾーン情報
    void ClearZoneObject(MentalMap::MentalZoneInfo& info);

    //! @brief 四分木
    //! @param center 現在の領域の中心
    //! @param size 現在の領域のサイズ
    //! @param points この領域に含まれる点
    //! @param current_depth 現在の階層
    //! @param max_depth 最大階層
    void Quadtree(float2 center, float2 size, const std::vector<float3>& points, int current_depth, int max_depth);

    //! @brief 現在のエリアを取得
    //! @return 現在のエリアのインデックス
    int GetCurrentArea() const
    {
        return check_area_;
    }

    //! @brief 最も近い点のインデックスを取得
    //! @return 最も近い点のインデックス
    int GetClosestPoint() const
    {
        return closest_point_;
    }

private:
    //! @brief 座標から見える領域の候補ポイントを取得
    //! @param points 領域内の点のリスト
    //! @param my_pos 自分の位置
    //! @param excluded_positions 除外する位置のリスト
    //! @param prevent_radius プレイヤーとの距離がこの半径以内の点
    //! @param search_angle 検索角度
    //! @param prevent_rate 減衰率
    //! @param check_behind 敵の背後もチェックするかどうか
    std::vector<float3> GetCandidatePoints(const std::vector<float3>& points,
                                           const float3& my_pos,
                                           const std::vector<float3>& excluded_positions,
                                           const float prevent_radius,
                                           const float search_angle,
                                           const float prevent_rate,
                                           const bool check_behind = false);

private:
    std::vector<float3> candidate_points_;   //<! 座標から見える領域の候補ポイント

    //! @brief 領域の分割
    void SplitArea();

    //! @brief 必要なデータの初期化
    void InitData();

    std::vector<std::pair<float3, float3>> mental_bounding_;   //!< 領域のバウンディングボックス

    std::vector<std::vector<int>> adjacent_areas_;   //!< 隣接領域

    float2 map_size_        = float2{500.0f, 500.0f};   //!< マップサイズ
    int split_time_         = 3;                        //!< 分割回数
    float split_area_range_ = 0.0f;                     //!< 分割領域の範囲
    std::vector<std::pair<float3, float3>> bound_box;   //!< 領域のバウンディングボックス

    bool has_target_   = false;                      //!< 目的地があるか
    float3 player_pos_ = float3{0.0f, 0.0f, 0.0f};   //!< プレイヤー位置

    std::vector<std::vector<float3>> split_area_points_;     //!< 分割された領域の点
    std::vector<std::vector<int>> split_area_points_color;   //!< 分割された領域の点の色
    float2 center_;                                          //!< マップ中心位置

    MentalMapBuilder::SaveData data_;                                //!< セーブ用データ
    std::vector<MentalMapBuilder::AreaPointData> area_point_data_;   //!< 領域可視性データ

    bool use_debug_     = false;   //!< デバッグ表示フラグ
    bool use_map_debug_ = false;   //!< マップデバッグ表示フラグ
    int check_area_     = 0;       //!< 今どの領域にいるか
    int closest_point_  = 0;       //!< 最も近い点
};

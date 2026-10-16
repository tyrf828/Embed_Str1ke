#pragma once
#include <System/Component/Component.h>
#include <RecastNavigation/Detour/DetourNavMesh.h>
#include <RecastNavigation/Detour/DetourNavMeshQuery.h>
#include <vector>

class Navmesh : public Component
{
public:
    BP_COMPONENT_DECL(Navmesh, u8"ナビメッシュ");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 描画
    void LateDraw() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief ターゲット設定
    //! @param obj ターゲットオブジェクト
    void SetTarget(const ObjectPtr& obj);

    //! @brief ターゲット設定
    //! @param pos ターゲット位置
    void SetTarget(const float3& pos);

    //! @brief ターゲットが存在するか
    //! @retval true 存在する
    //! @retval false 存在しない
    bool GetTarget() const
    {
        return !target_.expired();
    }

    //! @brief ターゲット位置を取得
    //! @return ターゲット位置
    float3 GetTargetPos() const
    {
        if(auto t = target_.lock())
        {
            return t->GetTranslate();
        }
        return target_pos_;
    }

    //! @brief パスを再計算
    void RebuildPath();

    //! @brief パスポイントを取得
    //! @return パスポイントのリスト
    const std::vector<float3>& GetPathPoints() const
    {
        return path_points_;
    }

    //! @brief 最寄りのナビメッシュ上の位置を取得
    //! @param pos 検索位置
    //! @return 最寄りの位置
    float3 GetNearestPos(float3 pos) const;

    //! @brief 有効な近傍ポリゴンが見つかった場合だけ位置を返す
    bool TryGetNearestPos(float3 pos, float3& result) const;

    //! @brief ステージオブジェクトを設定
    //! @param obj ステージオブジェクト
    void SetStage(ObjectPtr obj)
    {
        navmesh_anchor_ = obj;
    }

    //! @brief ナビメッシュを使用するか設定
    //! @param use 使用するか
    void UseNavmesh(bool use)
    {
        use_navmesh_ = use;
    }

    //! @brief ナビメッシュを使用しているか
    //! @retval true 使用している
    //! @retval false 使用していない
    bool IsUseNavmesh() const
    {
        return use_navmesh_;
    }

    //! @brief ナビメッシュ上の距離を取得
    //! @param target_pos 目標位置
    //! @return 距離
    float GetNavmeshDistance(const float3& target_pos);

    //! @brief ナビメッシュ上の距離を取得
    //! @param start_pos 開始位置
    //! @param target_pos 目標位置
    //! @return 距離
    float GetNavmeshDistance(const float3& start_pos, const float3& target_pos);

    bool IsInNavmesh(float3 start_pos, float3 end_pos);

private:
    //! @brief ナビメッシュをロード
    //! @param filepath ファイルパス
    void LoadNavmesh(const char* filepath);

    //! @brief パスを計算
    //! @param start_position 開始位置
    //! @param end_position 終了位置
    //! @retval true 計算成功
    //! @retval false 計算失敗
    bool ComputePath(float3& start_position, float3& end_position);

    //! @brief NavMesh デバッグ描画
    void DrawNavmeshDebug();

    //! @brief ポイントをスケーリング
    //! @param p ポイント
    //! @param s スケール
    //! @return スケーリングされたポイント
    float3 ScalePoint(const float3& p, float s) const;

    //! @brief ポイントを変換
    //! @param m 行列
    //! @param p ポイント
    //! @return 変換されたポイント
    float3 TransformPoint(const matrix& m, const float3& p) const;

    //! @brief 右手系から左手系への変換行列を取得
    //! @return 変換行列
    matrix RHtoLHMatrix() const;

    //! @brief 右手系から左手系へポイントを変換
    //! @param p ポイント
    //! @return 変換されたポイント
    float3 RHtoLHPoint(const float3& p) const;

private:
    //! @brief パス検索
    bool PathSearch(const float3& start_pos, const float3& end_pos, dtPolyRef& start_ref, dtPolyRef& end_ref, float* clamped_start, float* clamped_end) const;

    dtNavMesh* nav_mesh_   = nullptr;   //!< ナビメッシュ
    dtNavMeshQuery* query_ = nullptr;   //!< ナビメッシュクエリ
    dtQueryFilter filter_;              //!< デフォルトフィルタ

    std::vector<float3> path_points_;   //!< パスポイント
    int current_point_ = 0;             //!< 現在のポイントインデックス

    ObjectWeakPtr target_;                           //!< ターゲットオブジェクト
    bool has_target_   = false;                      //!< 目的地があるか
    bool use_navmesh_  = true;                       //!< ナビメッシュを使用するか
    float3 target_pos_ = float3{0.0f, 0.0f, 0.0f};   //!< 目的地

    float move_speed_;   //!< 移動速度

    float3 search_half_extents_ = {1.0f, 10.0f, 1.0f};   //!< 検索ボックス半径

    float repath_timer_    = 0.0f;   //!< 再計算タイマー
    float repath_interval_ = 0.2f;   //!< 再計算間隔 (秒)

    // デバック
    bool debug_draw_navmesh_ = false;   //!< ナビメッシュデバッグ描画フラグ

    ObjectWeakPtr navmesh_anchor_{};   //!< ナビメッシュアンカー

    float navmesh_unit_scale_ = 10.0f;   //!< ナビメッシュスケール（NavMeshからWorld座標への変換）

    dtPolyRef follow_ref_ = 0;   //!< 現在のポリゴン参照

    static constexpr int MAX_POLYS_    = 512;    //!< 最大ポリゴン数
    static constexpr int MAX_STRAIGHT_ = 2048;   //!< 最大直線パス数

    dtPolyRef polys_[MAX_POLYS_];                   //!< ポリゴンバッファ
    float straight_pts_[MAX_STRAIGHT_ * 3];         //!< 直線パスポイントバッファ
    unsigned char straight_flags_[MAX_STRAIGHT_];   //!< 直線パスフラグバッファ
    dtPolyRef straight_refs_[MAX_STRAIGHT_];        //!< 直線パスポリゴン参照バッファ

    float3 prev_position_ = {0.0f, 0.0f, 0.0f};   //!< 前回の位置
};

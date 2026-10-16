#pragma once
//===========================================================================
//! @file SpawnManager.h
//! @brief スポーンマネージャー
//===========================================================================
#include <vector>
#include <functional>

USING_PTR(SpawnManager);
USING_PTR(Spawner);

//---------------------------------------------------------------------------
//! @class SpawnManager
//! @brief スポーンマネージャー
//---------------------------------------------------------------------------
class SpawnManager : public Object
{
public:
    BP_OBJECT_DECL(SpawnManager, u8"スポーンマネージャー");

    //! @brief 初期化
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 描画
    void LateDraw() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief スポーンプレハブを追加
    //! @tparam T スポーンするオブジェクトの型
    template <typename T> void AddSpawnPrefab()
    {
        Prefab p;
        p.prefab_ = []() { return Scene::Object::Create<T>(); };
        spawn_prefab_.emplace_back(p);
    }

    //! @brief スポーンプールに追加
    //! @param object オブジェクト
    //! @return 自身のポインタ
    SpawnManagerPtr AddSpawnPool(ObjectPtr object);

    //! @brief スポナーを追加
    //! @param pos 座標
    //! @return 自身のポインタ
    SpawnManagerPtr AddSpawner(const float3& pos);

    //! @brief スポナーを追加
    //! @param data_path データパス
    //! @return 自身のポインタ
    SpawnManagerPtr AddSpawners(std::string_view data_path);

    //! @brief スポナーを追加
    //! @param spawners スポナー座標リスト
    //! @return 自身のポインタ
    SpawnManagerPtr AddSpawners(std::vector<float3> spawners);

    //! @brief スポーン距離を設定
    //! @param distance 距離
    void SetSpawnDistance(float distance)
    {
        spawn_distance_ = distance;
    }

    //! @brief スポーン時間を設定
    //! @param time 時間
    void SetSpawnTime(float time)
    {
        spawn_time_ = time;
    }

    //! @brief スポーンを実行
    //! @param check_view 画面内チェックを行うか
    //! @param check_distance 距離チェックを行うか
    void DoSpawn(bool check_view = true, bool check_distance = true);

    //! @brief スポーン処理
    void SpawnProcess();

    //! @brief スポーン初期化
    void InitSpawn();

    //! @brief スポーン停止
    //! @param is_stop 停止するか
    void StopSpawn(bool is_stop = true);

    //! @brief シーン内の最大オブジェクト数を設定
    //! @param max_objects 最大数
    void SetSceneObjectMax(int max_objects);

    //! @brief スポーン済みオブジェクトを設定
    //! @param obj オブジェクト
    //! @param index インデックス
    void SetSpawnedObject(ObjectPtr obj, int index);

    //! @brief スポーン済みオブジェクトを取得
    //! @param index インデックス
    //! @return オブジェクト
    ObjectPtr GetSpawnedObject(int index) const;

    //! @brief プールオブジェクトをクリア
    //! @param is_destroy_all 全て削除するか
    void ClearPoolObjects(bool is_destroy_all);

    //! @brief アクティブオブジェクトをクリア
    //! @param is_destroy_all 全て削除するか
    void ClearActiveObjects(bool is_destroy_all)
    {
        if(is_destroy_all)
        {
            for(auto& obj : active_objects_)
            {
                Scene::Object::Release(obj);
            }
        }
        active_objects_.clear();
    }

    //! @brief 全てのオブジェクトをクリア
    //! @param remove_self 自身を削除するか
    void ClearAllObjects(bool remove_self);

    //! @brief オブジェクトクリア開始
    void StartClearObjects();

    //! @brief 全てのスポナーを削除
    void RemoveAllSpawners();

    //! @brief スポナー数を取得
    //! @return スポナー数
    int GetSpawnerCount() const
    {
        return (int)spawners_data.size();
    }

    //! @brief スポナー座標を取得
    //! @param index インデックス
    //! @return 座標
    float3 GetSpawnerPosition(int index) const
    {
        if(index >= 0 && index < spawners_data.size())
        {
            return spawners_data[index].pos_;
        }
        return float3{0.0f, 0.0f, 0.0f};
    }

    void UseDebugDraw(bool use = true)
    {
        is_debug_draw_ = use;
    }

private:
    struct SpawnerData
    {
        float3 pos_;                     //!< スポナー座標
        ObjectWeakPtr spawned_object_;   //!< スポーン済みオブジェクト
        bool is_init_pos_ = false;       //!< 初期座標設定フラグ
        bool is_in_view_  = false;       //!< 画面内フラグ
    };
    static constexpr float EXPIRED_DISTANCE_ = 30.0f;   //!< 離脱距離
    std::vector<SpawnerData> spawners_data;             //!< スポナーデータ

    //! @brief スポナーでオブジェクトをスポーン
    //! @param spawner スポナーデータ
    void SpawnObjectAtSpawner(SpawnerData& spawner);

    struct Prefab
    {
        std::function<ObjectPtr()> prefab_;   //!< プレハブ
    };

    std::vector<Prefab> spawn_prefab_;        //!< スポーンプレハブ
    std::vector<ObjectPtr> object_pool_;      //!< オブジェクトプール
    std::vector<ObjectPtr> active_objects_;   // シーン内のアクティブなオブジェクト

    bool is_stop_           = false;    //!< 停止フラグ
    float spawn_distance_   = 300.0f;   //!< スポーン距離
    int max_active_objects_ = 15;       // シーン内の最大オブジェクト数
    float spawn_timer_      = 0.0f;     //!< スポーンタイマー
    float spawn_time_       = 2.0f;     //!< スポーン時間
    bool is_start_clear_    = false;    //!< クリア開始フラグ
    bool init_spawn_        = false;    //!< 初期スポーンフラグ
    bool is_debug_draw_     = false;    //!< デバッグ描画フラグ
};

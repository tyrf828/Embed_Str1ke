//===========================================================================
//! @file EnemyMentalManager.h
//! @brief メンタルマップマネージャー
//===========================================================================
#pragma once
#include <Game/Object/Enemy/MentalMap.h>
#include <unordered_map>

USING_PTR(Player);
USING_PTR(EnemyMentalManager);
USING_PTR(EnemyBase);

//---------------------------------------------------------------------------
//! @class EnemyMentalManager
//! @brief メンタルマップマネージャー
//---------------------------------------------------------------------------
class EnemyMentalManager : public Object
{
public:
    BP_OBJECT_DECL(EnemyMentalManager, u8"EnemyMentalManager");

    //! @brief 初期化
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief GUI
    void GUI() override;

    //! @brief 敵が向かうべきターゲット座標を取得する
    //! @param enemy_pos 敵の現在位置
    //! @param excluded_positions 除外する座標
    //! @return ターゲット座標
    float3 GetPreferPosition(const float3& enemy_pos, const std::vector<float3>& excluded_positions = {});

    //! @brief 自分は有効なゾーンに含まれるか判定
    //! @param enemy_pos 敵の位置
    //! @param target_pos ターゲットにしたい位置
    bool IsInValidEnemyZone(const float3& enemy_pos, const float3& target_pos);

    //! @brief 敵を登録
    //! @param enemy 登録する敵
    void RegisterEnemyList(EnemyBasePtr enemy);

    //! @brief 登録した敵を解除
    //! @param enemy 解除する敵
    void ReleaseEnemyList(EnemyBasePtr enemy)
    {
        enemies_.erase(std::remove(enemies_.begin(), enemies_.end(), enemy), enemies_.end());
        record_target_pos.erase(enemy);
    }

    //! @brief 敵のターゲット位置を記録
    //! @param enemy ターゲット位置を記録する敵
    float3 GetRecordTargetPos(EnemyBasePtr enemy);

    //! @brief 敵の位置からエリアインデックスを取得
    //! @param pos 位置
    //! @return エリアインデックス
    int GetAreaIndex(const float3& pos);

    //! @brief 指定された位置のゾーンレベルを取得
    //! @param area_index エリアインデックス
    //! @param pos 位置
    //! @return ゾーンレベル
    int GetZoneLevel(int area_index, const float3& pos);

private:
    static constexpr float UPDATE_INTERVAL_ = 1.0f;   //!< マップ更新間隔
    float update_timer_                     = 0.0f;   //!< マップ更新タイマー
    MentalMap::MentalZoneInfo zone_info_;             //!< メンタルゾーン情報

    std::vector<EnemyBasePtr> enemies_;   //!< 敵リスト

    int last_area         = -1;      //!< 前回のエリア
    bool is_zone_changed_ = false;   //!< ゾーンが変更されたかどうか

    std::unordered_map<EnemyBasePtr, float3> record_target_pos;   //!< 敵のターゲット位置の記録

    //! @brief 最も近い敵の位置を更新
    //! @param mental_map メンタルマップ
    //! @param player プレイヤー
    //! @param assigned_positions 既に割り当てられた位置
    void ClosestEnemyUpdate(MentalMapPtr mental_map, PlayerPtr player, std::vector<float3>& assigned_positions);
};

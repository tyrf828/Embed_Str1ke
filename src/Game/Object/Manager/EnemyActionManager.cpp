//===========================================================================
//! @file EnemyActionManager.cpp
//! @brief 敵の行動管理マネージャー
//===========================================================================
#include "EnemyActionManager.h"
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyThreat.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyAttack.h>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyAction.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Object/Player/Player.h>
#include <System/Component/ComponentModel.h>
#include <Game/Component/ComponentHP.h>
#include <unordered_map>
#include <Game/Component/Enemy/CloseEnemy/Action/CloseEnemyFindPlayer.h>
#include <Game/Component/Enemy/ActionBase/ActionAttackBase.h>
#include <Game/Component/Enemy/ActionBase/ActionThreatBase.h>
#include <Game/Component/Navmesh.h>

EnemyActionManager* EnemyActionManager::instance_ = nullptr;   //!< シングルトン

//! @brief 初期化
//! @return 初期化済み
bool EnemyActionManager::Init()
{
    Super::Init();

    // シングルトンの設定
    if(instance_ && instance_ != this)
    {
        Scene::Object::Release(shared_from_this());
        return true;
    }
    instance_     = this;
    change_timer_ = CHANGE_TIME_;

    return true;
}

//! @brief 更新
void EnemyActionManager::Update()
{
    Super::Update();

    if(is_stop_)
    {
        return;
    }

    bool have_attack_enemy = false;
    // 攻撃遅延リストの更新
    for(auto it = attack_delay_list_.begin(); it != attack_delay_list_.end();)
    {
        have_attack_enemy  = true;
        it->delay_time    -= GetDeltaTime();
        if(it->delay_time <= 0.0f)
        {
            // 攻撃状態に変更
            if(auto action = it->enemy->GetComponent<EnemyActionBase>())
            {
                action->SetAttack();
            }
            it = attack_delay_list_.erase(it);
        }
        else
        {
            it++;
        }
    }

    if(have_attack_enemy)
    {
        // 攻撃遅延中の敵がいる場合は処理しない
        return;
    }

    // 威嚇状態の敵がいるかチェック
    bool have_threat_enemy = false;
    for(auto enemy : Scene::Object::GetArray<EnemyBase>())
    {
        auto action = enemy->GetComponent<EnemyActionBase>();
        if(!action)
        {
            continue;
        }
        if(dynamic_cast<ActionThreatBase*>(action->GetCurrentAction().get()) != nullptr)
        {
            have_threat_enemy = true;
            break;
        }
    }

    // 威嚇状態の敵がいる場合、一定時間ごとに攻撃対象を変更
    if(have_threat_enemy)
    {
        change_timer_ -= GetDeltaTime();
        if(change_timer_ <= 0.0f)
        {
            change_timer_ = CHANGE_TIME_;
            ChangeNearestThreatEnemiesToAttack();
        }
    }
}

//! @brief 終了
void EnemyActionManager::Exit()
{
    Super::Exit();
    if(instance_ == this)
    {
        instance_ = nullptr;
    }
    enemy_vec_.clear();
    attack_delay_list_.clear();
}

//! @brief 攻撃を停止するか設定
//! @param is_stop 停止するか
//! @param clear_attack_enemy 攻撃候補の敵をクリアするか
void EnemyActionManager::StopAttack(bool is_stop, bool clear_attack_enemy)
{
    is_stop_ = is_stop;
    if(clear_attack_enemy)
    {
        for(auto list : attack_delay_list_)
        {
            if(auto action = list.enemy->GetComponent<EnemyActionBase>())
            {
                action->SetFindPlayer();
            }
        }

        // 攻撃遅延リストをクリア
        attack_delay_list_.clear();
    }
}

//! @brief 最も近い威嚇状態の敵を攻撃状態に変更
void EnemyActionManager::ChangeNearestThreatEnemiesToAttack()
{
    auto camera = Scene::Object::Get<Camera>();
    auto player = Scene::Object::Get<Player>();
    if(!camera || !player)
    {
        return;
    }

    // カメラ位置と前方向ベクトル
    float3 cam_pos     = camera->GetTranslate();
    float3 cam_forward = camera->GetFront();
    cam_forward        = normalize(cam_forward);

    // 画角内と外の敵
    std::vector<std::pair<EnemyBasePtr, float>> inside_list;
    std::vector<std::pair<EnemyBasePtr, float>> outside_list;
    int attack_count = 0;   // 既に攻撃中の敵

    for(auto enemy : Scene::Object::GetArray<EnemyBase>())
    {
        if(!enemy)
        {
            continue;
        }
        auto action = enemy->GetComponent<EnemyActionBase>();
        if(!action)
        {
            continue;
        }

        // 既に攻撃中の敵をカウント
        if(dynamic_cast<ActionAttackBase*>(action->GetCurrentAction().get()) != nullptr)
        {
            attack_count++;
            continue;
        }
        if(dynamic_cast<ActionThreatBase*>(action->GetCurrentAction().get()) == nullptr)
        {
            continue;
        }

        // 距離を計算
        float distance = 0.0f;
        if(auto nav = enemy->GetComponent<Navmesh>())
        {
            distance = nav->GetNavmeshDistance(player->GetTranslate());
        }
        else
        {
            distance = length(enemy->GetTranslate() - player->GetTranslate());
        }

        float3 vec = enemy->GetTranslate() - cam_pos;
        float3 dir = normalize(vec);
        float dotv = dot(dir, cam_forward);
        dotv       = std::clamp(dotv, -1.0f, 1.0f);
        // 内積を角度に変換
        float deg = acosf(dotv) * RadToDeg;

        // 画角内判定
        bool inside_fov = (deg <= camera->GetFOV() / 2.0f);
        if(inside_fov)
        {
            inside_list.push_back({enemy, distance});
        }
        else
        {
            outside_list.push_back({enemy, distance});
        }
    }

    // 攻撃可能な敵の数を計算
    int change_limit = ENEMY_ACTION_NUM_MAX_ - attack_count;
    if(change_limit <= 0)
    {
        return;   // すでに上限到達
    }

    // 近い順にする
    auto less = [](const auto& a, const auto& b) { return a.second < b.second; };
    std::sort(inside_list.begin(), inside_list.end(), less);
    std::sort(outside_list.begin(), outside_list.end(), less);

    int change_count = 0;

    // まず画角内を優先
    for(auto& p : inside_list)
    {
        if(change_count >= change_limit)
        {
            break;
        }

        // 距離が遠すぎる場合はスキップ
        const float EXPIRE_DISTANCE = 200.0f;
        if(p.second > EXPIRE_DISTANCE)
        {
            continue;
        }

        // 攻撃遅延リストに追加
        float delay = GetRandomF(ATTACK_DELAY_MIN_, ATTACK_DELAY_MAX_) * change_count;
        attack_delay_list_.push_back({p.first, delay});
        change_count++;
    }

    // 画角内で足りなければ画角外も入れる
    if(change_count <= 0)
    {
        for(auto& p : outside_list)
        {
            if(change_count >= change_limit)
            {
                break;
            }
            // 攻撃遅延リストに追加
            float delay = GetRandomF(ATTACK_DELAY_MIN_, ATTACK_DELAY_MAX_) * change_count;
            attack_delay_list_.push_back({p.first, delay});
            change_count++;
        }
    }
}

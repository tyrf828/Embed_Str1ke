//===========================================================================
//! @file BossActionManager.cpp
//! @brief ボスの動作管理マネージャー
//===========================================================================
#include "BossActionManager.h"
#include <limits>
#include <Game/Component/Enemy/Boss/BossAnimationController.h>
#include <Game/Component/Enemy/Boss/Action/BossAction.h>
#include <Game/Component/Enemy/Boss/Action/BossIdle.h>
#include <Game/Component/Enemy/Boss/Action/BossDashAttack.h>
#include <Game/Component/Enemy/Boss/Action/BossKick.h>
#include <Game/Component/Enemy/Boss/Action/BossJump.h>
#include <Game/Component/Enemy/Boss/Action/BossFallBack.h>
#include <Game/Component/Enemy/Boss/Action/BossGrabAttack.h>
#include <Game/Component/Enemy/Boss/Action/BossConfront.h>
#include <Game/Component/Enemy/Boss/Action/BossDamaged.h>
#include <Game/Component/Enemy/Boss/Action/BossBlast.h>
#include <Game/Object/Player/Player.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Input/InputKey.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Component/Player/UltimateAttack/UltimateAttack.h>
#include <Game/Component/Sound.h>
#include <Game/Component/Enemy/Boss/BossAI.h>

//! @brief 初期化
void BossActionManager::Init()
{
    Super::Init();
    if(use_onnx_ai_)
    {
        GetOwner()->AddComponent<BossAI>();
    }
    // 点数の初期化
    for(int i = 0; i < AttackType::MAX; i++)
    {
        attack_[i].type_         = (AttackType)i;
        attack_[i].score_        = 0.0f;
        attack_success_bouns_[i] = 1.0f;
    }
    last_attack_  = AttackType::NONE;
    hit_num_count = 0;
    think_timer_  = THINK_TIME_MAX_;
}

//! @brief 更新
void BossActionManager::Update()
{
    Super::Update();
    auto boss_action = GetOwner()->GetComponent<BossAction>();
    if(!boss_action)
    {
        return;
    }

    if(auto player = Scene::Object::Get<Player>())
    {
        auto hp = player->GetComponent<ComponentHP>();
        if(hp)
        {
            // プレイヤー死亡時は待機
            if(hp->GetHP() <= 0)
            {
                boss_action->SetAction(std::make_shared<BossIdle>());
                return;
            }
        }

        // プレイヤーが必殺技攻撃中は行動しない
        if(player->GetComponent<UltimateAttack>())
        {
            return;
        }
    }

    auto current_action = boss_action->GetCurrentAction();
    if(IsAction<BossIdle>(current_action))
    {
        boss_action->SetAction(std::make_shared<BossConfront>());
    }
    // 行動決定可能な状態か判定
    bool can_decide = IsAction<BossIdle>(current_action) || IsAction<BossConfront>(current_action) || IsAction<BossDamaged>(current_action);
    if(!can_decide)
    {
        return;
    }
    // 思考タイマー更新
    think_timer_ -= GetDeltaTime();
    if(think_timer_ > 0.0f)
    {
        return;
    }

    if(use_onnx_ai_)
    {
        auto onnx_ai = GetOwner()->GetComponent<BossAI>();
        if(!onnx_ai)
        {
            return;
        }

        // 推論に必要な情報を作成
        BossAI::Input input{};

        if(auto hp = GetOwner()->GetComponent<ComponentHP>())
        {
            input.boss_hp     = hp->GetHP();
            input.boss_max_hp = hp->GetMaxHP();
        }
        if(auto player = Scene::Object::Get<Player>())
        {
            if(auto hp = player->GetComponent<ComponentHP>())
            {
                input.player_hp     = hp->GetHP();
                input.player_max_hp = hp->GetMaxHP();
            }

            float3 boss_pos   = GetOwner()->GetTranslate();
            float3 player_pos = player->GetTranslate();
            float3 to_player  = player_pos - boss_pos;
            to_player.y       = 0.0f;
            auto col          = GetOwner()->GetComponent<ComponentCollisionCapsule>();
            auto player_col   = player->GetComponent<ComponentCollisionCapsule>();
            if(!col || !player_col)
            {
                return;
            }
            // プレイヤーとの距離計算
            player_distance_ = length(to_player) - col->GetRadius() - player_col->GetRadius();
            player_distance_ = std::max(0.0f, player_distance_);
            input.distance   = player_distance_;
        }

        input.last_attack = -1;
        input.hit_count   = hit_num_count;
        if(input.boss_max_hp <= 0 || input.player_max_hp <= 0 || input.boss_hp <= 0)
        {
            return;
        }
        if(last_attack_ != AttackType::NONE)
        {
            input.last_attack = (int)last_attack_;
        }
        for(int i = 0; i < AttackType::MAX; i++)
        {
            input.success_bonus[i] = attack_success_bouns_[i];
        }

        std::vector<float> input_data;
        onnx_ai->CreateInputToData(input, input_data);

        std::vector<float> output_data(5);
        onnx_ai->Reasoning(input_data, output_data);

        // 出力データを攻撃スコアに反映
        for(int i = 0; i < AttackType::MAX; i++)
        {
            attack_[i].score_ = output_data[i];
        }
    }
    else
    {
        // 採点用情報更新
        UpdateContext();

        // 各攻撃のスコア計算
        attack_[AttackType::KICK].score_          = ScoreKick();
        attack_[AttackType::GRAB].score_          = ScoreGrab();
        attack_[AttackType::DASH].score_          = ScoreDash();
        attack_[AttackType::FALLBACK_DASH].score_ = ScoreFallBack();
        attack_[AttackType::BLAST].score_         = ScoreBlast();
    }

    // 射程内の攻撃から最大値を選ぶ。負の推論値でも比較する。
    constexpr std::array<float, AttackType::MAX> min_distance = {0, 0, 15, 0, 0};
    constexpr std::array<float, AttackType::MAX> max_distance = {15, 50, 350, 15, 40};
    // 最高スコアの攻撃を選択
    Attack highest_attack = {AttackType::NONE, -std::numeric_limits<float>::infinity()};
    for(int i = 0; i < AttackType::MAX; i++)
    {
        if(player_distance_ >= min_distance[i] && player_distance_ <= max_distance[i] && std::isfinite(attack_[i].score_) &&
           attack_[i].score_ > highest_attack.score_)
        {
            highest_attack = attack_[i];
        }
    }

    // 実行できる候補があれば、思考タイマーの間隔で攻撃する。
    if(highest_attack.type_ != AttackType::NONE)
    {
        for(int i = 0; i < AttackType::MAX; i++)
        {
            if(attack_[i].type_ == highest_attack.type_)
            {
                attack_[i].score_ = 0.0f;
                Execute(attack_[i]);
                last_attack_ = attack_[i].type_;
                break;
            }
        }
        think_timer_ = THINK_TIME_MAX_;
    }
    else
    {
        // 攻撃不可なら対峙を維持
        if(!IsAction<BossConfront>(current_action))
        {
            boss_action->SetAction(std::make_shared<BossConfront>());
        }
        // 射程外では既存の対峙アクションで前進して距離を詰める。
        if(auto confront = std::dynamic_pointer_cast<BossConfront>(boss_action->GetCurrentAction()))
        {
            if(!confront->GetConfrontAction(BossConfront::ConfrontType::FORWARD))
            {
                confront->SetConfrontAction(BossConfront::ConfrontType::FORWARD);
            }
        }
        // 攻撃可能になるまで思考時間を短縮
        think_timer_ = THINK_TIME_MAX_ * 0.1f;
    }
}

//! @brief 終了
void BossActionManager::Exit()
{
    Super::Exit();
}

//! @brief 攻撃成功時の処理
//! @param attack_type 成功した攻撃タイプ
void BossActionManager::OnAttackSuccess(AttackType attack_type)
{
    // 攻撃成功ボーナス加算
    attack_success_bouns_[attack_type] += 0.05f;
    // ある程度上限を設ける
    attack_success_bouns_[attack_type] = std::min(attack_success_bouns_[attack_type], 1.2f);
}

//! @brief 攻撃失敗時の処理
//! @param attack_type 失敗した攻撃タイプ
void BossActionManager::OnAttackFailed(AttackType attack_type)
{
    // 攻撃成功ボーナス減算
    attack_success_bouns_[attack_type] -= 0.05f;
    // ある程度下限を設ける
    attack_success_bouns_[attack_type] = std::max(attack_success_bouns_[attack_type], 0.8f);
}

//! @brief GUI表示処理
void BossActionManager::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("BossActionManager"))
        {
            ImGui::Text("dist: %.2f", player_distance_);
            ImGui::Text("think: %.2f", think_timer_);
            ImGui::Text("hits since attack: %d", hit_num_count);

            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//! @brief 採点用情報更新
void BossActionManager::UpdateContext()
{
    // プレイヤー位置と角度を自前で計算
    auto player = Scene::Object::Get<Player>();
    if(!player)
    {
        return;
    }

    float3 boss_pos   = GetOwner()->GetTranslate();
    float3 player_pos = player->GetTranslate();
    float3 to_player  = player_pos - boss_pos;
    to_player.y       = 0.0f;
    auto col          = GetOwner()->GetComponent<ComponentCollisionCapsule>();
    auto player_col   = player->GetComponent<ComponentCollisionCapsule>();
    if(col && player_col)
    {
        // プレイヤーとの距離計算
        player_distance_ = length(to_player) - col->GetRadius() - player_col->GetRadius();
        player_distance_ = std::max(0.0f, player_distance_);
    }

    // プレイヤーの回復動作中か
    player_is_regen_ = player->IsRegenerateHP();
}

//! @brief プレイヤー回復スコア
float BossActionManager::PlayerRegenerateScore()
{
    // プレイヤーが回復動作中なら高スコア
    if(player_is_regen_)
    {
        return 1.0f;
    }
    return 0.0f;
}

//! @brief -1.0から1.0にクランプ
float BossActionManager::ClampM1P1(float value)
{
    value = std::clamp(value, -1.0f, 1.0f);
    return value;
}

//! @brief 距離スコア計算
float BossActionManager::DistanceScore(float distance, float range, float penalty_rate)
{
    // 指定距離範囲内は高スコア
    float min_distance = distance - range;
    float max_distance = distance + range;
    if(player_distance_ >= min_distance && player_distance_ <= max_distance)
    {
        return 1.0f;
    }
    // 範囲外はペナルティ
    if(player_distance_ < min_distance)
    {
        float diff = player_distance_ - min_distance;
        float p    = diff * penalty_rate;
        p          = ClampM1P1(p);
        return p;
    }
    if(player_distance_ > max_distance)
    {
        float diff = max_distance - player_distance_;
        float p    = diff * penalty_rate;
        p          = ClampM1P1(p);
        return p;
    }

    return 1.0f;
}

//! @brief ボスHPスコア
float BossActionManager::BossHPScore(HP_TYPE hp_type)
{
    auto hp = GetOwner()->GetComponent<ComponentHP>();
    if(!hp)
    {
        return 0.0f;
    }
    // HP割合計算
    float hp_percent = (float)(hp->GetHP()) / hp->GetMaxHP() * 100;
    HP_TYPE current_hp_type;
    if(hp_percent >= (float)HP_MID)
    {
        current_hp_type = HP_HIGH;
    }
    else if(hp_percent >= (float)HP_LOW)
    {
        current_hp_type = HP_MID;
    }
    else
    {
        current_hp_type = HP_LOW;
    }

    // 指定したタイプと一致するなら高スコア
    if(current_hp_type == hp_type)
    {
        return 1.0f;
    }

    // 一致しないなら減点
    return 0.7f;
}

//! @brief プレイヤーHPスコア
float BossActionManager::PlayerHPScore(HP_TYPE hp_type)
{
    auto player = Scene::Object::Get<Player>();
    if(!player)
    {
        return 0.0f;
    }
    auto hp = player->GetComponent<ComponentHP>();
    if(!hp)
    {
        return 0.0f;
    }
    // HP割合計算
    float hp_percent = (float)(hp->GetHP()) / hp->GetMaxHP() * 100;
    HP_TYPE current_hp_type;
    if(hp_percent >= (float)HP_MID)
    {
        current_hp_type = HP_HIGH;
    }
    else if(hp_percent >= (float)HP_LOW)
    {
        current_hp_type = HP_MID;
    }
    else
    {
        current_hp_type = HP_LOW;
    }
    // 指定したタイプと一致するなら高スコア
    if(current_hp_type == hp_type)
    {
        return 1.0f;
    }
    // 一致しないなら減点
    return 0.5f;
}

//! @brief 前回の攻撃開始以降の被弾
float BossActionManager::GettingHitScore(float rate)
{
    float count = (float)std::clamp(hit_num_count, 0, 10);
    return std::clamp(1.0f + count * (rate - 1.0f), 0.8f, 1.2f);
}

//! @brief 実行
void BossActionManager::Execute(Attack& attack)
{
    auto boss_action = GetOwner()->GetComponent<BossAction>();
    if(!boss_action)
    {
        return;
    }
    attack.score_ = 0.0f;   // スコアリセット
    switch(attack.type_)
    {
    case AttackType::KICK:
        boss_action->SetAction(std::make_shared<BossKick>());
        break;
    case AttackType::GRAB:
        boss_action->SetAction(std::make_shared<BossGrabAttack>());
        break;
    case AttackType::DASH:
        boss_action->SetAction(std::make_shared<BossDashAttack>());
        break;
    case AttackType::FALLBACK_DASH:
        boss_action->SetAction(std::make_shared<BossFallBack>());
        break;
    case AttackType::BLAST:
        boss_action->SetAction(std::make_shared<BossBlast>());
        break;
    default:
        return;
    }
    // 今回の判断には前回以降の回数を使い、新しい攻撃の開始から数え直す。
    hit_num_count = 0;
}

//! @brief キックスコア
float BossActionManager::ScoreKick()
{
    float score  = 0.0f;
    score       += DistanceScore(8.0f, 5.0f);
    score       += BossHPScore(HP_TYPE::HP_HIGH);
    score       += PlayerHPScore(HP_TYPE::HP_HIGH);
    score       *= attack_success_bouns_[AttackType::KICK];
    score       *= GettingHitScore(0.98f);
    if(last_attack_ == AttackType::KICK)
    {
        score *= 0.5f;
    }
    if(player_distance_ > 15.0f)
    {
        // 距離外はスコア無効
        score = 0.0f;
    }
    return score;
}

//! @brief 掴み攻撃スコア
float BossActionManager::ScoreGrab()
{
    float score  = 0.0f;
    score       += DistanceScore(35.0f, 15.0f);
    score       += BossHPScore(HP_TYPE::HP_MID);
    score       += PlayerHPScore(HP_TYPE::HP_HIGH);
    score       *= attack_success_bouns_[AttackType::GRAB];
    score       *= GettingHitScore(1.05f);
    if(last_attack_ == AttackType::GRAB)
    {
        score *= 0.7f;
    }
    return score;
}

//! @brief 突進攻撃スコア
float BossActionManager::ScoreDash()
{
    float score  = 0.0f;
    score       += DistanceScore(200.0f, 150.0f);
    score       += BossHPScore(HP_TYPE::HP_HIGH);
    score       += PlayerHPScore(HP_TYPE::HP_MID);
    score       *= attack_success_bouns_[AttackType::DASH];
    score       *= GettingHitScore(0.95f);
    if(last_attack_ == AttackType::DASH)
    {
        score *= 0.8f;
    }
    if(player_distance_ < 15.0f)
    {
        // 距離外はスコア無効
        score = 0.0f;
    }
    return score;
}

//! @brief 後退からの突進攻撃スコア
float BossActionManager::ScoreFallBack()
{
    float score  = 0.0f;
    score       += DistanceScore(5.0f, 5.0f);
    score       += BossHPScore(HP_TYPE::HP_LOW);
    score       += PlayerHPScore(HP_TYPE::HP_MID);
    score       *= attack_success_bouns_[AttackType::FALLBACK_DASH];
    score       *= GettingHitScore(1.1f);
    if(last_attack_ == AttackType::FALLBACK_DASH)
    {
        score *= 0.2f;
    }
    return score;
}

//! @brief 爆発攻撃スコア
float BossActionManager::ScoreBlast()
{
    float score  = 0.0f;
    score       += DistanceScore(20.0f, 5.0f);
    score       += BossHPScore(HP_TYPE::HP_MID);
    score       += PlayerHPScore(HP_TYPE::HP_HIGH);
    score       *= attack_success_bouns_[AttackType::BLAST];
    score       *= GettingHitScore(1.1f);
    if(last_attack_ == AttackType::BLAST)
    {
        score *= 0.5f;
    }
    return score;
}

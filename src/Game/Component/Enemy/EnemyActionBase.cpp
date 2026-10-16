//===========================================================================
//! @file EnemyActionBase.cpp
//! @brief 敵の行動ベースクラス
//===========================================================================
#include "EnemyActionBase.h"
#include <Game/Object/Player/Player.h>
#include <System/Component/ComponentCollisionSphere.h>

//! @brief オーナーを設定
//! @param owner オーナーオブジェクト
void EnemyActionBase::SetOwner(ObjectPtr owner)
{
    owner_ = owner;
}

//! @brief オーナーを取得
//! @return オーナーオブジェクト
ObjectPtr EnemyActionBase::GetOwner() const
{
    return owner_;
}

//! @brief アクションを設定
//! @param action 設定するアクション
void EnemyActionBase::SetAction(std::shared_ptr<EnemyActionBase> action)
{
    if(now_action_)
    {
        now_action_->Exit();
        if(now_action_->CanTransit())
        {
            prev_action_ = now_action_;
        }
    }
    now_action_ = action;
    if(now_action_)
    {
        now_action_->SetOwner(GetOwnerPtr());
        now_action_->Init();
    }
}

//! @brief プレイヤーとの角度を取得
//! @return プレイヤーとの角度
float EnemyActionBase::FacePlayerDegree() const
{
    // 前ベクトルを取得
    auto axis = -GetOwner()->GetVectorAxisZ();
    // プレイヤーを取得
    auto player = Scene::Object::Get<Player>();
    if(!player)
    {
        return -1;
    }
    // プレイヤーへのベクトル
    float3 vec = player->GetTranslate() - GetOwner()->GetTranslate();
    vec        = normalize(vec);

    // 内積
    float dot_ = dot(axis, vec);
    dot_       = (dot_ / (length(axis) * length(vec)));
    dot_       = std::clamp((float)dot_, -1.0f, 1.0f);

    // 角度に変換
    float deg = (acosf(dot_)) * RadToDeg;

    return deg;
}

//! @brief プレイヤーとの距離を取得
//! @return プレイヤーとの距離
float EnemyActionBase::GetPlayerDistance() const
{
    // プレイヤーを取得
    auto player = Scene::Object::Get<Player>();
    if(!player)
    {
        return -1;
    }

    // 距離を計算
    float distance = length(player->GetTranslate() - GetOwner()->GetTranslate());
    return distance;
}

//! @brief プレイヤーの方向への角度を取得
//! @return プレイヤーの方向への角度
float EnemyActionBase::GetPlayerDegree() const
{
    float3 boss_pos   = GetOwner()->GetTranslate();
    float3 player_pos = GetPlayer()->GetTranslate();
    float3 player_dir = player_pos - boss_pos;

    float3 boss_front = -GetOwner()->GetVectorAxisZ();
    boss_front.y      = 0.0f;
    // 内積
    float d            = dot(normalize(boss_front), normalize(player_dir));
    d                  = std::clamp(d, -1.0f, 1.0f);
    float face_degree_ = std::acos(d) * RadToDeg;
    return face_degree_;
}

//! @brief プレイヤーを取得
//! @return プレイヤーオブジェクト
ObjectPtr EnemyActionBase::GetPlayer() const
{
    ObjectWeakPtr player = Scene::Object::Get<Player>();
    return player.lock();
}

//! @brief 攻撃コリジョンを追加
//! @param node アタッチするノード
//! @param radius 半径
void EnemyActionBase::AddSphereCollision(std::string_view node, float radius)
{
    auto col = GetOwner()->AddComponent<ComponentCollisionSphere>();
    col->SetCollisionGroup(ComponentCollision::CollisionGroup::ETC);
    col->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER);
    col->SetOverlapCollisionGroup((u32)ComponentCollision::CollisionGroup::PLAYER | (u32)ComponentCollision::CollisionGroup::GROUND |
                                  (u32)ComponentCollision::CollisionGroup::WEAPON | (u32)ComponentCollision::CollisionGroup::ETC |
                                  (u32)ComponentCollision::CollisionGroup::ENEMY_WEAPON | (u32)ComponentCollision::CollisionGroup::ENEMY);
    col->SetRadius(radius);
    col->AttachToModel(node);
}

//! @brief 攻撃コリジョンを削除
void EnemyActionBase::RemoveSphereCollisions()
{
    auto cols = GetOwner()->GetComponents<ComponentCollisionSphere>();
    for(auto& col : cols)
    {
        GetOwner()->RemoveComponent(col);
    }
}

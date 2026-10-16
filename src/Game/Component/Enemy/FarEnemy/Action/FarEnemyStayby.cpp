//===========================================================================
//! @file FarEnemyStayby.cpp
//! @brief 敵の待機準備コンポーネント
//===========================================================================
#include "FarEnemyStayby.h"
#include <Game/Component/Enemy/FarEnemy/FarEnemyAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Object/Manager/SpawnManager.h>
#include <Game/Object/Enemy/FarEnemy/FarEnemy.h>
#include <Game/Component/DissolveEffect.h>

//! @brief 初期化
void FarEnemyStayby::Init()
{
    Super::Init();

    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動を停止
        move->SetVelocity(float3(0.0f, 0.0f, 0.0f));
        move->SetMove(false);
    }
    if(auto anim_control = GetOwner()->GetComponent<FarEnemyAnimationController>())
    {
        anim_control->SetFlag(FarEnemyAnimationController::STATE_BIT::IDLE);
    }

    // スポーンプールに追加
    auto spawn_manager = Scene::Object::Get<SpawnManager>("EnemySpawnManager");
    if(!spawn_manager)
    {
        spawn_manager = Scene::Object::Get<SpawnManager>();
    }
    if(spawn_manager)
    {
        spawn_manager->AddSpawnPool(GetOwner());
    }

    // 負荷軽減のため当たり判定削除
    if(GetOwner()->GetComponent<ComponentCollisionCapsule>())
    {
        GetOwner()->RemoveComponent<ComponentCollisionCapsule>();
    }
    // 待機場所へ移動
    GetOwner()->SetTranslate(float3(100.0f, 10.0f, 100.0f));
}

//! @brief 更新
void FarEnemyStayby::Update()
{
    Super::Update();

    auto owner = dynamic_cast<FarEnemy*>(GetOwner().get());
    if(auto anim_control = owner->GetComponent<FarEnemyAnimationController>())
    {
        // 負荷軽減のため、アニメーション開始直後に更新、描画を停止
        if(anim_control->IsAnimationTimeInRange(0.1f))
        {
            owner->SetStatus(Object::StatusBit::NoUpdate, true);
            owner->SetStatus(Object::StatusBit::NoDraw, true);
            owner->GetWeaponPtr()->SetStatus(Object::StatusBit::NoUpdate, true);
            owner->GetWeaponPtr()->SetStatus(Object::StatusBit::NoDraw, true);
        }
    }
}

//! @brief 終了
void FarEnemyStayby::Exit()
{
    Super::Exit();
    auto owner = dynamic_cast<FarEnemy*>(GetOwner().get());
    // 見た目に関する削除ここで解除
    if(owner->GetComponent<DissolveEffect>())
    {
        owner->RemoveComponent<DissolveEffect>();
    }
    if(auto weapon = owner->GetWeaponPtr()->GetComponent<DissolveEffect>())
    {
        owner->GetWeaponPtr()->RemoveComponent<DissolveEffect>();
    }

    // 更新、描画を再開
    owner->SetStatus(Object::StatusBit::NoUpdate, false);
    owner->SetStatus(Object::StatusBit::NoDraw, false);
    owner->GetWeaponPtr()->SetStatus(Object::StatusBit::NoUpdate, false);
    owner->GetWeaponPtr()->SetStatus(Object::StatusBit::NoDraw, false);

    // 移動を再開
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        move->SetMove(true);
    }
}

//! @brief GUI表示処理
void FarEnemyStayby::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("EnemyStayby"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

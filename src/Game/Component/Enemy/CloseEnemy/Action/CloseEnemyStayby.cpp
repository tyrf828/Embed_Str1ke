//===========================================================================
//! @file CloseEnemyStayby.cpp
//! @brief 敵の待機準備コンポーネント
//===========================================================================
#include "CloseEnemyStayby.h"
#include <Game/Component/Enemy/EnemyMove.h>
#include <Game/Component/Enemy/CloseEnemy/CloseEnemyAnimationController.h>
#include <System/Component/ComponentModel.h>
#include <Game/Object/Enemy/CloseEnemy/CloseEnemy.h>
#include <Game/Object/Manager/SpawnManager.h>
#include <Game/Component/DissolveEffect.h>

//! @brief 初期化
void CloseEnemyStayby::Init()
{
    Super::Init();
    // 移動を停止
    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        move->SetVelocity(float3(0.0f, 0.0f, 0.0f));
        move->SetMove(false);
    }
    if(auto anim_control = GetOwner()->GetComponent<CloseEnemyAnimationController>())
    {
        anim_control->SetFlag(CloseEnemyAnimationController::STATE_BIT::IDLE);
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

    // コリジョン削除
    if(GetOwner()->GetComponent<ComponentCollisionCapsule>())
    {
        GetOwner()->RemoveComponent<ComponentCollisionCapsule>();
    }
    // 待機場所へ移動
    GetOwner()->SetTranslate(float3(100.0f, 10.0f, 100.0f));
}

//! @brief 更新
void CloseEnemyStayby::Update()
{
    Super::Update();
    auto owner = dynamic_cast<CloseEnemy*>(GetOwner().get());
    if(auto anim_control = owner->GetComponent<CloseEnemyAnimationController>())
    {
        // 処理負荷軽減のためアニメーション開始直後に更新、描画を停止
        if(anim_control->IsAnimationTimeInRange(0.1f) && owner)
        {
            owner->SetStatus(Object::StatusBit::NoUpdate, true);
            owner->SetStatus(Object::StatusBit::NoDraw, true);
            owner->GetWeaponPtr()->SetStatus(Object::StatusBit::NoUpdate, true);
            owner->GetWeaponPtr()->SetStatus(Object::StatusBit::NoDraw, true);
        }
    }
}

//! @brief 終了
void CloseEnemyStayby::Exit()
{
    Super::Exit();
    auto owner = dynamic_cast<CloseEnemy*>(GetOwner().get());
    if(owner->GetComponent<DissolveEffect>())
    {
        // ディゾルブエフェクト削除
        owner->RemoveComponent<DissolveEffect>();
    }
    if(auto weapon = owner->GetWeaponPtr()->GetComponent<DissolveEffect>())
    {
        // ディゾルブエフェクト削除
        owner->GetWeaponPtr()->RemoveComponent<DissolveEffect>();
    }

    // 更新、描画を再開
    owner->SetStatus(Object::StatusBit::NoUpdate, false);
    owner->SetStatus(Object::StatusBit::NoDraw, false);
    owner->GetWeaponPtr()->SetStatus(Object::StatusBit::NoUpdate, false);
    owner->GetWeaponPtr()->SetStatus(Object::StatusBit::NoDraw, false);

    if(auto move = GetOwner()->GetComponent<EnemyMove>())
    {
        // 移動再開
        move->SetMove(true);
    }
}

//! @brief GUI表示処理
void CloseEnemyStayby::GUI()
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

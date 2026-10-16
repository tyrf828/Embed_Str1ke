//===========================================================================
//! @file BossDeath.cpp
//! @brief ボス死亡コンポーネント
//===========================================================================
#include "BossDeath.h"
#include <System/Component/ComponentModel.h>
#include <Game/Component/Enemy/Boss/BossAnimationController.h>
#include <Game/Component/Enemy/EnemyMove.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Object/Enemy/EnemyBase.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
void BossDeath::Init()
{
    Super::Init();

    if(auto anim_control = GetOwner()->GetComponent<BossAnimationController>())
    {
        // アニメーションを設定
        anim_control->SetFlag(BossAnimationController::STATE_BIT::DEATH);
    }
    if(auto hp = GetOwner()->GetComponent<ComponentHP>())
    {
        // HPを0に固定
        hp->SetHP(0);
    }

    if(auto col = GetOwner()->GetComponent<ComponentCollisionCapsule>())
    {
        // 衝突グループを地面に変更
        col->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::GROUND);
    }
    destroy_count_ = DESTROY_TIME_MAX_;
    if(auto se = GetOwner()->GetComponent<Sound>())
    {
        // 死亡音を再生
        se->PlayNoSame("death");
    }
}

//! @brief 更新
void BossDeath::Update()
{
    Super::Update();

    if(auto hp = GetOwner()->GetComponent<ComponentHP>())
    {
        hp->SetHP(0);
    }

    // ロックポイントを解放
    if(auto owner = dynamic_cast<EnemyBase*>(GetOwner().get()))
    {
        owner->ClearEnemyLockPoint();
    }

    if(auto model = GetOwner()->GetComponent<ComponentModel>())
    {
        // アニメーションを再生中なら待機
        if(model->IsPlaying())
        {
            return;
        }
        // 一定時間後に削除
        destroy_count_ -= GetDeltaTime();
        if(destroy_count_ <= 0)
        {
            Scene::Object::Release(GetOwner()->shared_from_this());
        }
    }
}
//! @brief 終了
void BossDeath::Exit()
{
    Super::Exit();
    if(GetOwner()->GetComponent<ComponentCollisionCapsule>())
    {
        GetOwner()->RemoveComponent<ComponentCollisionCapsule>();
    }
}

//! @brief GUI表示処理
void BossDeath::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("BossDeath"))
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

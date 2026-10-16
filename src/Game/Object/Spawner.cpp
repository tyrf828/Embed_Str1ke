//===========================================================================
//! @file Spawner.cpp
//! @brief スポーナー
//===========================================================================
#include "Spawner.h"

//! @brief 初期化
//! @return 初期化済み
bool Spawner::Init()
{
    Super::Init();
    return true;
}

//! @brief 更新
void Spawner::Update()
{
    Super::Update();

    if(auto obj = spawned_object_.lock())
    {
        float distance = length(obj->GetTranslate() - GetTranslate());
        if(!is_init_pos_)
        {
            // 初期位置に移動させる処理
            const float INIT_DISTANCE = 3.0f;
            if(distance < INIT_DISTANCE)
            {
                // 十分近づいたら描画を有効化し、初期化完了とする
                obj->SetStatus(Object::StatusBit::NoDraw, false);
                is_init_pos_ = true;
            }
            else
            {
                // まだ遠い場合は強制的に位置を合わせ、描画と更新を無効化
                obj->SetStatus(Object::StatusBit::NoUpdate, false);
                obj->SetStatus(Object::StatusBit::NoDraw, true);
                obj->SetTranslate(GetTranslate());
            }
        }
        else if(distance > EXPIRED_DISTANCE_)
        {
            // 一定距離離れたらスポナーの管理から外す
            spawned_object_.reset();
        }
    }
}

//! @brief 描画
void Spawner::Draw()
{
    Super::Draw();
}

//! @brief 終了
void Spawner::Exit()
{
    Super::Exit();
}

//! @brief GUI
void Spawner::GUI()
{
    Super::GUI();
    ImGui::Begin(GetName().data());
    {
        ImGui::Separator();
        if(auto obj = spawned_object_.lock())
        {
            ImGui::Text(u8"オブジェクト: %s", obj->GetName().data());
        }
        else
        {
            ImGui::Text(u8"オブジェクト: なし");
        }
    }
    ImGui::End();
}

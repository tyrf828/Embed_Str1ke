//===========================================================================
//! @file CircularBoundary.cpp
//! @brief 円形境界コンポーネント実装
//===========================================================================
#include "CircularBoundary.h"

//! @brief 初期化
void CircularBoundary::Init()
{
    Super::Init();
}

//! @brief 更新
void CircularBoundary::Update()
{
    Super::Update();
    for(auto& weak_obj : objects_)
    {
        auto obj = weak_obj.lock();
        if(!obj)
        {
            continue;
        }
        // オブジェクトの現在位置を取得
        float3 obj_pos = obj->GetTranslate();
        // 中心からオブジェクトへのベクトルを計算
        float3 vec = obj_pos - center_;
        vec.y      = 0.0f;   // Y軸は無視
        // 距離を計算
        float distance = length(vec);
        // 半径を超えている場合、境界上に押し戻す
        if(distance > radius_)
        {
            // 正規化して方向を取得
            float3 direction = normalize(vec);
            // 境界上の位置を計算
            float3 boundary_pos = center_ + direction * radius_;
            boundary_pos.y      = obj_pos.y;
            // オブジェクトを境界上に移動
            obj->SetTranslate(boundary_pos);
        }
    }
}

//! @brief 終了
void CircularBoundary::Exit()
{
    Super::Exit();
}

//! @brief GUI表示
void CircularBoundary::GUI()
{
    Super::GUI();

    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("CircularBoundary"))
        {
            ImGui::DragFloat(u8"半径", &radius_, 0.5f, 0.0f, 1000.0f);

            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

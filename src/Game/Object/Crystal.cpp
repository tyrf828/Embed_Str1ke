//===========================================================================
//! @file Crystal.cpp
//! @brief クリスタル
//===========================================================================
#include "Crystal.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentAttachModel.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
//! @return 初期化済み
bool Crystal::Init()
{
    Super::Init();
    AddComponent<ComponentModel>("data/Game/Model/Crystal/Amethyst Crystal E 1k.mv1");

    AddComponent<Sound>()->Load({
        {"drop", "data/Game/SE/CutScene/茶碗を置く.mp3", 256.0f, 1.0f},
    });

    return true;
}

//! @brief 衝突時の処理
//! @param hit_info 衝突したオブジェクトの情報
void Crystal::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    Super::OnHit(hit_info);
}

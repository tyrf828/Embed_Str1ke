//===========================================================================
//! @file AnimationBoss.cpp
//! @brief ボス(アニメーションの演出だけ用)
//===========================================================================
#include "AnimationBoss.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/CutScene/Animation/CutSceneBossAnimationController.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
//! @return 初期化済み
bool AnimationBoss::Init()
{
    Super::Init();
    // モデルコンポーネント
    AddComponent<ComponentModel>("data/Game/Model/Boss/Nightshade J Friedrich.mv1")->SetScaleAxisXYZ(float3{0.13f, 0.13f, 0.13f});

    // カプセルコリジョン
    AddComponent<ComponentCollisionCapsule>()->SetRadius(2.0f)->SetHeight(10.0f)->UseGravity();

    // アニメーションコントローラー
    AddComponent<CutSceneBossAnimationController>();

    // サウンドコンポーネント
    AddComponent<Sound>()->Load({
        {"stand up", "data/Game/SE/CutScene/衣擦れ1.mp3",        256.0f, 1.0f},
        {"attack",   "data/Game/SE/CutScene/07_cool_attack.wav", 256.0f, 1.0f},
    });

    return true;
}

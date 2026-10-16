//===========================================================================
//! @file AnimationPlayer.cpp
//! @brief 操作不可のプレイヤー(アニメーションの演出だけ用)
//===========================================================================
#include "AnimationPlayer.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Component/CutScene/Animation/CutScenePlayerAnimationController.h>
#include <Game/Component/Sound.h>

//! @brief 初期化
//! @return 初期化済み
bool AnimationPlayer::Init()
{
    Super::Init();
    // モデルコンポーネント
    AddComponent<ComponentModel>("data/Game/Model/Player/Arissa.mv1")->SetScaleAxisXYZ(float3{0.13f, 0.13f, 0.13f});

    // カプセルコリジョン
    AddComponent<ComponentCollisionCapsule>()->SetRadius(2.0f)->SetHeight(10.0f)->UseGravity();

    // アニメーションコントローラー
    AddComponent<CutScenePlayerAnimationController>();

    // サウンドコンポーネント
    AddComponent<Sound>()->Load({
        {"walk",   "data/Game/SE/CutScene/革靴で歩く.mp3",  256.0f, 1.5f},
        {"attack", "data/Game/SE/CutScene/軽いパンチ2.mp3", 256.0f, 1.0f},
    });

    return true;
}

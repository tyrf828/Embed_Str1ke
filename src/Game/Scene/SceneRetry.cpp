//===========================================================================
//! @file SceneRetry.cpp
//! @brief リザルトシーン
//===========================================================================
#include "SceneRetry.h"
#include <Game/Object/RetryUI.h>

//! @brief 初期化
//! @return 初期化済み
bool SceneRetry::Init()
{
    Super::Init();

    // リザルト
    Scene::Object::Create<RetryUI>("RetryUI", true);

    return true;
}

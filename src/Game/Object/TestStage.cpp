#include "TestStage.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionModel.h>
#include <Game/Component/SphericalMaskTransition.h>
#include <Game/Component/DissolveEffect.h>
bool TestStage::Init()
{
    Super::Init();
    auto model = AddComponent<ComponentModel>("data/Game/SwordBout/Stage/Stage00.mv1");
    auto s     = AddComponent<ComponentCollisionModel>();
    s->AttachToModel();

    // Dissolve エフェクトを追加
    //AddComponent<DissolveEffect>();
    //->StartDissolve(3.0f, false);
    return true;
}

void TestStage::Update()
{
    Super::Update();
}

void TestStage::PostUpdate()
{
    Super::PostUpdate();
}

void TestStage::Exit()
{
    Super::Exit();
}

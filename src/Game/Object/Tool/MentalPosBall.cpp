//===========================================================================
//! @file MentalPosBall.cpp
//! @brief メンタルマップ用の当たり判定
//===========================================================================
#include "MentalPosBall.h"
#include <System/Debug/DebugCamera.h>
#include <System/Component/ComponentCollisionLine.h>
#include <System/Component/ComponentModel.h>
#include <fstream>
#include <System/Component/ComponentCollisionModel.h>
#include <Game/Object/Tool/MentalMapBuilder.h>

//! @brief 初期化
//! @return 初期化済み
bool MentalPosBall::Init()
{
    Super::Init();

    return true;
}

//! @brief 更新後
void MentalPosBall::Update()
{
    Super::Update();

    MV1_COLL_RESULT_POLY hit_poly{};
    auto model = Scene::Object::Get<Object>("Stage")->GetComponent<ComponentModel>();
    hit_poly   = MV1CollCheck_Line(model->GetModel(), -1, cast(GetTranslate()), cast(GetTranslate() + float3{0.0f, -1000.0f, 0.0f}));

    if(hit_poly.HitFlag)
    {
        Scene::Object::Get<MentalMapBuilder>()->AddMentalMapPoint(cast(hit_poly.HitPosition));
    }
    Scene::Object::Release(shared_from_this());
}

//! @brief 遅延描画
void MentalPosBall::LateDraw()
{
    Super::LateDraw();
    SetUseLighting(false);
    DrawSphere3D(cast(GetTranslate()), 1.0f, 16, GetColor(255, 255, 255), GetColor(255, 255, 255), false);
    SetUseLighting(true);
}

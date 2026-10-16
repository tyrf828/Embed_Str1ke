//===========================================================================
//! @file UltimateAttack.cpp
//! @brief 必殺技コンポーネント
//===========================================================================
#include "UltimateAttack.h"
#include "UltimateDash.h"

//! @brief 初期化
void UltimateAttack::Init()
{
    Super::Init();

    SetAction(std::make_shared<UltimateDash>());
}

//! @brief 更新
void UltimateAttack::Update()
{
    Super::Update();
    now_action_->Update();
}

//! @brief 描画
void UltimateAttack::LateDraw()
{
    Super::LateDraw();
    now_action_->LateDraw();
}

//! @brief GUI表示処理
void UltimateAttack::GUI()
{
    Super::GUI();
    now_action_->GUI();
}

//! @brief 終了
void UltimateAttack::Exit()
{
    Super::Exit();
    now_action_->Exit();
}

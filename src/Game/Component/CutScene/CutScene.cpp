//===========================================================================
//! @file CutScene.cpp
//! @brief カットシーン制御
//===========================================================================
#include "CutScene.h"
#include <Game/Component/CutScene/EnterBoss/CutScenePlayerWalkIn.h>
#include <Game/Component/UI/AutoDrawText.h>

//! @brief 初期化
void CutScene::Init()
{
    Super::Init();
}

//! @brief 更新
void CutScene::Update()
{
    Super::Update();
    if(now_state_)
    {
        now_state_->Update();
    }
}

//! @brief 遅延描画
void CutScene::LateDraw()
{
    Super::LateDraw();
    if(now_state_)
    {
        now_state_->LateDraw();
    }
}

//! @brief GUI表示
void CutScene::GUI()
{
    Super::GUI();
    if(now_state_)
    {
        now_state_->GUI();
    }
}

//! @brief 終了処理
void CutScene::Exit()
{
    Super::Exit();
    if(now_state_)
    {
        now_state_->Exit();
    }
}

//! @brief 名前テキストを作成
//! @param name 名前
void CutSceneBase::CreateNameText(std::string_view name)
{
    name_text_ = info_.owner_->AddComponent<AutoDrawText>();
    name_text_->SetAlignment(AutoDrawText::TextAlignment::CENTER)
        ->SetText(name)
        ->SetDrawBox({WINDOW_W * 0.4f, WINDOW_H * 0.83f}, {WINDOW_W * 0.6f, WINDOW_H * 0.87f})
        ->SetFontSize(18)
        ->SetDrawSpeed(0.0f)
        ->StartDraw();
}

//! @brief 名前テキストを削除
void CutSceneBase::ClearNameText()
{
    if(name_text_)
    {
        info_.owner_->RemoveComponent(name_text_);
        name_text_ = nullptr;
    }
}

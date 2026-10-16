#pragma once
//===========================================================================
//! @file TutorialRegen.h
//! @brief チュートリアル回復
//===========================================================================
#include "Game/Component/Tutorial/Tutorial.h"

USING_PTR(AutoDrawText);

//---------------------------------------------------------------------------
//! @class TutorialRegen
//! @brief チュートリアル回復
//---------------------------------------------------------------------------
class TutorialRegen : public TutorialBase
{
public:
    BP_COMPONENT_DECL(TutorialRegen, u8"チュートリアル回復");

    //! @brief 初期化
    void Init(const ProtectInfo& info) override;

    //! @brief 更新
    void Update() override;

    //! @brief 遅延描画
    void LateDraw() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 終了
    void Exit() override;

private:
    AutoDrawTextPtr message_text_     = nullptr;   //!< メッセージテキスト
    AutoDrawTextPtr quest_regen_text_ = nullptr;   //!< クエスト回復テキスト
    int regen_ghandle_                = -1;        //!< 回復ボタンハンドル
};

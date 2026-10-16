#pragma once
//===========================================================================
//! @file TutorialEnd.h
//! @brief チュートリアル終了
//===========================================================================
#include "Game/Component/Tutorial/Tutorial.h"

USING_PTR(AutoDrawText);

//---------------------------------------------------------------------------
//! @class TutorialEnd
//! @brief チュートリアル終了
//---------------------------------------------------------------------------
class TutorialEnd : public TutorialBase
{
public:
    BP_COMPONENT_DECL(TutorialEnd, u8"チュートリアル終了");

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
    AutoDrawTextPtr text_;            //!< テキスト
    int current_message_index_ = 0;   //!< 現在のメッセージインデックス
};

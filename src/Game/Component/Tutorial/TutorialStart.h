#pragma once
//===========================================================================
//! @file TutorialStart.h
//! @brief チュートリアルスタート
//===========================================================================
#include "Game/Component/Tutorial/Tutorial.h"

USING_PTR(AutoDrawText);

//---------------------------------------------------------------------------
//! @class TutorialStart
//! @brief チュートリアルスタート
//---------------------------------------------------------------------------
class TutorialStart : public TutorialBase
{
public:
    BP_COMPONENT_DECL(TutorialStart, u8"チュートリアルスタート");

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
    AutoDrawTextPtr auto_draw_text_ = nullptr;   //!< 自動描画テキスト
    int current_message_index_      = 0;         //!< 現在のメッセージインデックス
};

#pragma once
//===========================================================================
//! @file TutorialMove.h
//! @brief チュートリアル移動
//===========================================================================
#include "Game/Component/Tutorial/Tutorial.h"

USING_PTR(AutoDrawText);

//---------------------------------------------------------------------------
//! @class TutorialMove
//! @brief チュートリアル移動
//---------------------------------------------------------------------------
class TutorialMove : public TutorialBase
{
public:
    BP_COMPONENT_DECL(TutorialMove, u8"チュートリアル移動");

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
    AutoDrawTextPtr message_text_    = nullptr;   //!< メッセージテキスト
    AutoDrawTextPtr quest_move_text_ = nullptr;   //!< クエスト移動テキスト
    AutoDrawTextPtr quest_run_text_  = nullptr;   //!< クエスト走りテキスト
    int move_ghandle_                = -1;        //!< 移動ボタンハンドル
    int run_ghandle_                 = -1;        //!< 走りボタンハンドル
};

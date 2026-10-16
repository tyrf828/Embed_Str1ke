#pragma once
//===========================================================================
//! @file TutorialAttachAttack.h
//! @brief チュートリアルアタッチ攻撃
//===========================================================================
#include "Game/Component/Tutorial/Tutorial.h"

USING_PTR(AutoDrawText);
USING_PTR(SpawnManager);

//---------------------------------------------------------------------------
//! @class TutorialAttachAttack
//! @brief チュートリアルアタッチ攻撃
//---------------------------------------------------------------------------
class TutorialAttachAttack : public TutorialBase
{
public:
    BP_COMPONENT_DECL(TutorialAttachAttack, u8"チュートリアルアタッチ攻撃");

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
    std::array<ObjectPtr, 4> attach_weapon_;   //!< アタッチ武器
    AutoDrawTextPtr quest_attach_text_;        //!< クエストテキスト
    AutoDrawTextPtr quest_scan_text_;          //!< クエストテキスト
    int current_message_index_ = 0;            //!< 現在のメッセージインデックス
    int attach_ghandle_        = -1;           //!< アタッチボタンハンドル
    int scan_ghandle_          = -1;           //!< スキャンボタンハンドル
};

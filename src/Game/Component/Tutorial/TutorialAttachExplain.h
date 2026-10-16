#pragma once
//===========================================================================
//! @file TutorialAttachExplain.h
//! @brief チュートリアルアタッチ攻撃説明
//===========================================================================
#include "Game/Component/Tutorial/Tutorial.h"

USING_PTR(AutoDrawText);
USING_PTR(SpawnManager);
USING_PTR(BlackScreen);

//---------------------------------------------------------------------------
//! @class TutorialAttachExplain
//! @brief チュートリアルアタッチ攻撃説明
//---------------------------------------------------------------------------
class TutorialAttachExplain : public TutorialBase
{
public:
    BP_COMPONENT_DECL(TutorialAttachExplain, u8"チュートリアルアタッチ攻撃説明");

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
    AutoDrawTextPtr message_text_;       //!< メッセージテキスト
    int current_message_index_ = 0;      //!< 現在のメッセージインデックス
    SpawnManagerPtr spawn_manager_;      //!< スポーンマネージャー
    ObjectPtr show_camera_;              //!< 表示カメラ
    BlackScreenPtr transition_screen_;   //!< フェード画面
};

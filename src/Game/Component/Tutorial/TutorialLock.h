//===========================================================================
//! @file TutorialLock.h
//! @brief チュートリアルカメラロック
//===========================================================================
#pragma once
#include "Game/Component/Tutorial/Tutorial.h"

USING_PTR(AutoDrawText);

//---------------------------------------------------------------------------
//! @class TutorialLock
//! @brief チュートリアルカメラロック
//---------------------------------------------------------------------------
class TutorialLock : public TutorialBase
{
public:
    BP_COMPONENT_DECL(TutorialLock, u8"チュートリアルカメラロック");

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
    ObjectPtr show_camera_;            //!< 表示カメラ
    AutoDrawTextPtr message_text_;     //!< メッセージテキスト
    int current_message_index_ = 0;    //!< 現在のメッセージインデックス
    std::array<ObjectPtr, 2> enemy_;   //!< 敵
};

#pragma once
//===========================================================================
//! @file TutorialAttack.h
//! @brief チュートリアル攻撃
//===========================================================================
#include "Game/Component/Tutorial/Tutorial.h"

USING_PTR(AutoDrawText);

//---------------------------------------------------------------------------
//! @class TutorialAttack
//! @brief チュートリアル攻撃
//---------------------------------------------------------------------------
class TutorialAttack : public TutorialBase
{
public:
    BP_COMPONENT_DECL(TutorialAttack, u8"チュートリアル攻撃");

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
    AutoDrawTextPtr message_text_;               //!< メッセージテキスト
    AutoDrawTextPtr quest_attack_text_;          //!< クエストテキスト
    static constexpr float TEXT_SPEED_ = 0.1f;   //!< テキスト速度
    int attack_ghandle_                = -1;     //!< 攻撃ボタンハンドル
};

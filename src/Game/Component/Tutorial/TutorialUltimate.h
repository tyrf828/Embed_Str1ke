#pragma once
//===========================================================================
//! @file TutorialUltimate.h
//! @brief チュートリアル必殺技
//===========================================================================
#include "Game/Component/Tutorial/Tutorial.h"

USING_PTR(AutoDrawText);

//---------------------------------------------------------------------------
//! @class TutorialUltimate
//! @brief チュートリアル必殺技
//---------------------------------------------------------------------------
class TutorialUltimate : public TutorialBase
{
public:
    BP_COMPONENT_DECL(TutorialUltimate, u8"チュートリアル必殺技");

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
    AutoDrawTextPtr quest_ult_text_  = nullptr;              //!< クエスト必殺技テキスト
    AutoDrawTextPtr quest_lock_text_ = nullptr;              //!< クエストロックテキスト
    int current_message_index_       = 0;                    //!< 現在のメッセージインデックス
    int ult_ghandle_                 = -1;                   //!< 必殺技ボタンハンドル
    int lock_ghandle_                = -1;                   //!< ロックボタンハンドル
    float3 player_start_pos_         = {0.0f, 0.0f, 0.0f};   //!< プレイヤー開始位置
    float3 player_start_rot_         = {0.0f, 0.0f, 0.0f};   //!< プレイヤー開始回転
    std::vector<ObjectPtr> enemy_;                           //!< 敵
    bool is_success_ = false;                                //!< 成功したか
};

#pragma once
//===========================================================================
//! @file AttackBase.h
//! @brief プレイヤーの攻撃コンポーネントの基底クラス
//===========================================================================
#include <Game/Component/Player/AnimationController.h>

USING_PTR(Player);

//---------------------------------------------------------------------------
//! @class AttackBase
//! @brief 攻撃コンポーネントの基底クラス
//---------------------------------------------------------------------------
class AttackBase : public Component
{
public:
    BP_COMPONENT_DECL(AttackBase, u8"攻撃ベースコンポーネント");

    //! @brief 初期化
    //! @param anim_flag  設定するアニメションフラグ
    //! @param atk        攻撃力
    //! @param sound_name 音名
    void Initialize(AnimationController::STATE_BIT anim_flag, int atk, std::string sound_name);

    //! @brief 更新
    void Update() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 攻撃キーを無効化か
    //! @param disable_key 無効化するか
    void DisableAttackButton(bool disable_key)
    {
        disabled_attack_key_ = disable_key;
    }

protected:
    bool go_next_attack_      = false;   //!< 次の攻撃に移行するか
    bool attack_tigger_       = false;   //!< 攻撃判定に入るか
    bool disabled_attack_key_ = false;   //< 攻撃キーを無効化するか

    //! @brief 攻撃キーが押されているか
    //! @return 攻撃キーが押されているか
    bool PressedAttackButton()
    {
        return (IsKeyDown(KEY_INPUT_C) || IsMouseDown(MOUSE_INPUT_LEFT) || IsPadOn(PAD_ID::PAD_X)) && !disabled_attack_key_;
    }

    //! @brief 攻撃判定を付ける
    //! @param start_time 開始時間
    void SetHit(const float& start_time);

    //! @brief 攻撃判定を消す
    //! @param limit_time 終了時間
    void SetNoHit(const float& limit_time);

    //! @brief プレイヤを取得
    //! @return プレイヤのポインタ
    PlayerPtr GetPlayerOwner();
};

#pragma once
//===========================================================================
//! @file AnimationPlayer.h
//! @brief 操作不可のプレイヤー(アニメーションの演出だけ用)
//===========================================================================

USING_PTR(AnimationPlayer);

//---------------------------------------------------------------------------
//! @class AnimationPlayer
//! @brief 操作不可のプレイヤー(アニメーションの演出だけ用)
//---------------------------------------------------------------------------
class AnimationPlayer : public Object
{
public:
    BP_OBJECT_DECL(AnimationPlayer, "AnimationPlayer");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;
};

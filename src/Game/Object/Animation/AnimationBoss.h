#pragma once
//===========================================================================
//! @file AnimationBoss.h
//! @brief ボス(アニメーションの演出だけ用)
//===========================================================================

USING_PTR(AnimationBoss);

class AnimationBoss : public Object
{
public:
    BP_OBJECT_DECL(AnimationBoss, "AnimationBoss");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;
};

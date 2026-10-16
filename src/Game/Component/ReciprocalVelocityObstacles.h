#pragma once
//===========================================================================
//! @file ReciprocalVelocityObstacles.h
//! @brief 敵同士衝突回避アルゴリズム
//===========================================================================
#include <Game/Object/Enemy/EnemyBase.h>

//---------------------------------------------------------------------------
//! @class RVO
//! @brief 敵同士衝突回避アルゴリズム
//---------------------------------------------------------------------------
class RVO : public Component
{
public:
    BP_COMPONENT_DECL(RVO, u8"敵同士衝突回避アルゴリズム");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 描画
    void LateDraw() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief ベクトル長を最大値以内に収める
    //! @param[in] v ベクトル
    //! @param[in] max_length 最大長さ
    //! @return ベクトル
    float3 ClampLength(const float3& v, float max_length)
    {
        float len = dot(v, v);
        len       = sqrtf(len);
        if(len > max_length)
        {
            return normalize(v) * max_length;
        }
        return v;
    }

    //! @brief 速度を調整
    //! @param other 他の敵
    //! @return 調整された速度
    float3 AdjustVelocity(EnemyBase* other);

private:
    float3 preferred_velocity_          = {0, 0, 0};   //!< 優先速度
    std::vector<EnemyBase*> enemy_list_ = {nullptr};   //!< 敵リスト
    float bias_                         = 0.0f;        //!< バイアス
    bool use_debug_                     = false;       //!< デバッグ使用フラグ
};

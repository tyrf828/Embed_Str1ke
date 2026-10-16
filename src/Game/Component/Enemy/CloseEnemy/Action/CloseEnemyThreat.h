#pragma once
//===========================================================================
//! @file CloseEnemyThreat.h
//! @brief 敵の威嚇コンポーネント
//===========================================================================
#include <Game/Component/Enemy/ActionBase/ActionThreatBase.h>

//---------------------------------------------------------------------------
//! @class CloseEnemyThreat
//! @brief 敵の威嚇コンポーネント
//---------------------------------------------------------------------------
class CloseEnemyThreat : public ActionThreatBase
{
public:
    BP_COMPONENT_DECL(CloseEnemyThreat, u8"敵の威嚇コンポーネント");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 威嚇最大距離を取得
    //! @return 威嚇最大距離
    float GetThreatMaxDistance() const override
    {
        return THREAT_MAX_DISTANCE_;
    }

    //! @brief 威嚇最小距離を取得
    //! @return 威嚇最小距離
    float GetThreatMinDistance() const override
    {
        return THREAT_MIN_DISTANCE_;
    }

    //! @brief 威嚇最大距離を取得
    //! @return 威嚇最大距離
    static float GetThreatMaxDistanceStatic()
    {
        return THREAT_MAX_DISTANCE_;
    }

    //! @brief 威嚇最小距離を取得
    //! @return 威嚇最小距離
    static float GetThreatMinDistanceStatic()
    {
        return THREAT_MIN_DISTANCE_;
    }

private:
    static constexpr float THREAT_MAX_DISTANCE_    = 70.0f;   //!< 威嚇最大距離
    static constexpr float THREAT_MIN_DISTANCE_    = 20.0f;   //!< 威嚇最小距離
    static constexpr float THREAT_ADJUST_DISTANCE_ = 20.0f;   //!< 調整距離
    static constexpr float STRAFE_SPEED_           = 0.2f;    //!< 横移動速度
    static constexpr float CHANGE_DIR_MAX_         = 2.0f;    //!< 方向転換最大時間
};

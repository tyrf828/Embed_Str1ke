#pragma once
//===========================================================================
//! @file Spawner.h
//! @brief スポーナー
//===========================================================================

USING_PTR(Spawner);

//---------------------------------------------------------------------------
//! @class Spawner
//! @brief スポーナー
//---------------------------------------------------------------------------
class Spawner : public Object
{
public:
    BP_OBJECT_DECL(Spawner, u8"スポーナー");
    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 描画
    void Draw() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI
    void GUI() override;

    //! @brief スポーン済みオブジェクトを設定
    //! @param obj オブジェクト
    void SetSpawnedObject(ObjectPtr obj)
    {
        spawned_object_ = obj;
    }

    //! @brief スポーン済みオブジェクトを取得
    //! @return オブジェクト
    ObjectPtr GetSpawnedObject() const
    {
        return spawned_object_.lock();
    }

private:
    ObjectWeakPtr spawned_object_;                      //!< スポーン済みオブジェクト
    static constexpr float EXPIRED_DISTANCE_ = 30.0f;   //!< 離脱距離
    bool is_init_pos_                        = false;   //!< 初期位置設定済みか
};

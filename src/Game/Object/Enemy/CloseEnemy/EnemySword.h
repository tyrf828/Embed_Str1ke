#pragma once
//===========================================================================
//! @file EnemySword.h
//! @brief 敵の武器（剣）
//===========================================================================
USING_PTR(EnemySword);

//---------------------------------------------------------------------------
//! @class EnemySword
//! @brief 敵の武器（剣）
//---------------------------------------------------------------------------
class EnemySword : public Object
{
public:
    BP_OBJECT_DECL(EnemySword, u8"敵の武器");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 衝突時の処理
    //! @param hit_info 衝突したオブジェクトの情報
    void OnHit(const ComponentCollision::HitInfo& hit_info) override;

    //! @brief 更新後
    void PostUpdate() override;

    //! @brief オーナーの設定
    void SetOwner(ObjectPtr owner)
    {
        owner_ = owner;
    }

    //! @brief モデルのアタッチ
    //! @param node アタッチするノード名
    void AttachModel(std::string_view node);

    //! @brief 攻撃判定の追加
    void AddCollision();

    //! @brief 攻撃判定の削除
    void RemoveCollision();

    //! @brief 攻撃判定の有効無効設定
    //! @param enable 有効にするならtrue
    void SetHitEnable(bool enable = true);

private:
    ObjectWeakPtr owner_;   //!< オーナー
};

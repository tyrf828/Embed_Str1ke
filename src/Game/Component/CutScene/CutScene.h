#pragma once
//===========================================================================
//! @file CutScene.h
//! @brief カットシーン制御
//===========================================================================
#include <System/Component/Component.h>

USING_PTR(CutScene);
USING_PTR(CutSceneBase);
USING_PTR(AutoDrawText);

//----------------------------------------------------------
//! @class CutSceneBase
//! @brief カットシーンの基底クラス
//----------------------------------------------------------
class CutSceneBase : public Component
{
public:
    //! @brief カットシーンで使用する情報
    struct ProtectInfo
    {
        ObjectWeakPtr player_;   //!< プレイヤー
        ObjectWeakPtr boss_;     //!< ボス
        ObjectWeakPtr camera_;   //!< カメラ
        ObjectPtr owner_;        //!< オーナー
    };

    //! @brief 初期化
    //! @param info カットシーン情報
    virtual void Init(const ProtectInfo& info)
    {
        info_ = info;
    }
    //! @brief 更新
    virtual void Update() {}
    //! @brief 遅延描画
    virtual void LateDraw() {}
    //! @brief GUI表示
    virtual void GUI() {}
    //! @brief 終了処理
    virtual void Exit() {}

    //! @brief カットシーンを切り替え
    //! @tparam T 変更先のカットシーンクラス
    template <typename T> void ChangeCutScene();

protected:
    ProtectInfo info_;          //!< カットシーン情報
    float scene_time_ = 0.0f;   //!< シーン経過時間

    //! @brief 名前テキストを作成
    //! @param name 名前
    void CreateNameText(std::string_view name);
    //! @brief 名前テキストを削除
    void ClearNameText();
    AutoDrawTextPtr name_text_ = nullptr;   //!< 名前テキストコンポーネント
};

//----------------------------------------------------------
//! @class CutScene
//! @brief カットシーン制御コンポーネント
//----------------------------------------------------------
class CutScene : public Component
{
public:
    BP_COMPONENT_DECL(CutScene, u8"カットシーン制御コンポーネント");

    //! @brief 初期化
    void Init() override;
    //! @brief 更新
    void Update() override;
    //! @brief 遅延描画
    void LateDraw() override;
    //! @brief GUI表示
    void GUI() override;
    //! @brief 終了処理
    void Exit() override;

    //! @brief 状態を設定
    //! @param state 設定する状態
    void SetState(std::shared_ptr<CutSceneBase> state)
    {
        if(now_state_)
        {
            now_state_->Exit();
        }
        now_state_ = state;
        if(now_state_)
        {
            info_.owner_ = GetOwnerPtr();
            now_state_->Init(info_);
        }
    }

    //! @brief プレイヤーを設定
    //! @param player プレイヤーオブジェクト
    //! @return 自身のポインタ
    CutScenePtr SetPlayer(const ObjectPtr player)
    {
        info_.player_ = player;
        return std::dynamic_pointer_cast<CutScene>(shared_from_this());
    }

    //! @brief ボスを設定
    //! @param boss ボスオブジェクト
    //! @return 自身のポインタ
    CutScenePtr SetBoss(const ObjectPtr boss)
    {
        info_.boss_ = boss;
        return std::dynamic_pointer_cast<CutScene>(shared_from_this());
    }

    //! @brief カメラを設定
    //! @param camera カメラオブジェクト
    //! @return 自身のポインタ
    CutScenePtr SetCamera(const ObjectPtr camera)
    {
        info_.camera_ = camera;
        return std::dynamic_pointer_cast<CutScene>(shared_from_this());
    }

private:
    std::shared_ptr<CutSceneBase> now_state_ = nullptr;   //!< 現在の状態
    CutSceneBase::ProtectInfo info_;                      //!< カットシーン情報
};

//! @brief カットシーンを切り替え
//! @tparam T 変更先のカットシーンクラス
template <typename T> void CutSceneBase::ChangeCutScene()
{
    if(auto owner = info_.owner_)
    {
        // カットシーンを変更
        if(auto cutscene = owner->GetComponent<CutScene>())
        {
            cutscene->SetState(std::make_shared<T>());
        }
    }
}

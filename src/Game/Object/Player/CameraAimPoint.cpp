//===========================================================================
//! @file CameraAimPoint.cpp
//! @brief カメラが見る場所
//===========================================================================
#include "CameraAimPoint.h"
#include "Player.h"
#include <Game/Object/Enemy/CloseEnemy/CloseEnemy.h>
#include <Game/Object/Enemy/EnemyLockPoint.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <Game/Component/ComponentCameraController.h>

//! @brief 初期化
//! @return 初期化済み
bool CameraAimPoint::Init()
{
    Super::Init();
    // カメラが見る位置を設定
    if(auto player = Scene::Object::Get<Player>("Player"))
    {
        SetTranslate(player->GetTranslate() + offset_);
    }

    // カメラコントローラを追加
    auto controller = AddComponent<ComponentCameraController>();
    controller->SetUseInput(true);
    controller->SetCameraRotateLimit(89.0f, -89.0f);
    return true;
}

//! @brief 更新
void CameraAimPoint::Update()
{
    Super::Update();

    if(auto player = Scene::Object::Get<Object>("Player"))
    {
        if(!bind_player_)
        {
            return;
        }
        // プレイヤーの頭上位置
        player_pos_ = {player->GetTranslate() + offset_};
        // カメラがプレイヤーへのベクトル
        float3 vec = player_pos_ - GetTranslate();

        // カメラが見る位置とプレイヤーの距離
        float distance = sqrtf((player_pos_.x - GetTranslate().x) * (player_pos_.x - GetTranslate().x) +
                               (player_pos_.y - GetTranslate().y) * (player_pos_.y - GetTranslate().y) +
                               (player_pos_.z - GetTranslate().z) * (player_pos_.z - GetTranslate().z));

        // 見る位置の移動
        if(distance > (float)FLT_EPSILON)
        {
            // 見る位置を移動
            AddTranslate(vec * CAMERA_MOVE_SPEED_);
        }
        else
        {
            // プレイヤーの位置に設定
            SetTranslate(player_pos_);
        }
    }
}

//! @brief プレイヤーにバインドするか
//! @retval true バインドする
//! @retval false バインドしない
void CameraAimPoint::SetBindPlayer(bool bind)
{
    bind_player_ = bind;
}

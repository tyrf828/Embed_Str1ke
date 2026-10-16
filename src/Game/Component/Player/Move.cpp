//===========================================================================
//! @file Move.cpp
//! @brief プレイヤーの移動コンポーネント
//===========================================================================
#include "Move.h"
#include <Game/Component/Player/AnimationController.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Component/Sound.h>
#include <algorithm>

//! @brief 初期化
void Move::Init()
{
    Super::Init();
}

//! @brief 更新
void Move::Update()
{
    Super::Update();

    // カメラを取得
    if(auto camera = Scene::Object::Get<Camera>())
    {
        // 移動を初期化
        dir_ = {0.0f, 0.0f, 0.0f};

        // 前キーを押した処理
        if(key_status_[KEY_FRONT])
        {
            // カメラの前の平面ベクトルに移動
            dir_ += camera->GetPlaneFront();
        }
        // 後ろキーを押した処理
        if(key_status_[KEY_BACK])
        {
            // カメラの後ろの平面ベクトルに移動
            dir_ -= camera->GetPlaneFront();
        }
        // 右キーを押した処理
        if(key_status_[KEY_RIGHT])
        {
            // カメラの右の平面ベクトルに移動
            dir_ += camera->GetPlaneRight();
        }
        // 左キーを押した処理
        if(key_status_[KEY_LEFT])
        {
            // カメラの左の平面ベクトルに移動
            dir_ -= camera->GetPlaneRight();
        }

        // 4方向の移動を設定
        if(key_status_[KEY_RIGHT])
        {
            walk_status_ = RIGHT;
        }
        if(key_status_[KEY_LEFT])
        {
            walk_status_ = LEFT;
        }
        if(key_status_[KEY_BACK])
        {
            walk_status_ = BACK;
        }
        if(key_status_[KEY_FRONT])
        {
            walk_status_ = FRONT;
        }

        // コントローラーを使う場合はアナログで返す
        if(GetConnecetdPadNum() != -1)
        {
            float x = GetPadInputAnalogLX();
            float y = GetPadInputAnalogLY();
            if(std::abs(x) > 0.1f || std::abs(y) > 0.1f)
            {
                dir_ = camera->GetPlaneRight() * x + camera->GetPlaneFront() * -y;
            }
        }

        // 移動を正規化
        if((float)length(dir_) > 0.0f)
        {
            dir_ = normalize(dir_);
        }
    }

    bool has_input = (float)length(dir_) > 0.0f;

    // 移動処理
    if(enabled_)
    {
        if(has_input)
        {
            speed_ratio_ += move_speed_ * GetDeltaTime() * acceleration_time_;
            speed_ratio_  = std::min(speed_ratio_, 1.0f);

            // 入力方向へ加速
            velocity_ += dir_ * speed_ratio_;
            // 方向を記録
            last_dir_ = dir_;
        }
        else
        {
            // 減速処理
            float speed = (float)length(velocity_);
            if(speed > 0.0f)
            {
                speed_ratio_ -= move_speed_ * GetDeltaTime() * damping_time_;
                speed_ratio_  = std::max(speed_ratio_, 0.0f);

                if(speed_ratio_ <= FLT_EPSILON)
                {
                    velocity_ = {0, 0, 0};
                }
                else
                {
                    velocity_ -= dir_ * speed_ratio_;
                    // 方向を記録
                    last_dir_ = normalize(velocity_);
                }
            }
            // 速度が小さい場合は方向ステータス初期化
            if((float)length(velocity_) <= FLT_EPSILON)
            {
                walk_status_ = DEFAULT;
            }
        }

        // 最大速度制限
        float speed = (float)length(velocity_);
        if(speed > move_speed_)
        {
            velocity_ = normalize(velocity_) * move_speed_;
            speed     = move_speed_;
        }

        // 速度があるなら移動
        if(speed > FLT_EPSILON)
        {
            GetOwner()->AddTranslate(velocity_, true);
        }

        // アニメーション・SE制御
        if(auto anim = GetOwner()->GetComponent<AnimationController>())
        {
            if(speed > FLT_EPSILON)
            {
                // 歩きフラグを設定 / 待機フラグを消す
                anim->SetFlag(AnimationController::STATE_BIT::WALK);
                anim->ResetFlag(AnimationController::STATE_BIT::IDLE);
                // 今のアニメーションが歩きならSE再生
                if(anim->GetNowPlaying((int)AnimationController::STATE::WALK))
                {
                    if(auto se = GetOwner()->GetComponent<Sound>())
                    {
                        se->PlayNoSame("walk");
                    }
                }
                else
                {
                    if(auto se = GetOwner()->GetComponent<Sound>())
                    {
                        se->Stop("walk");
                    }
                }
            }
            else
            {
                // 停止時SE停止
                if(auto se = GetOwner()->GetComponent<Sound>())
                {
                    se->Stop("walk");
                }
                // 待機フラグ設定 / 歩きフラグ解除
                anim->SetFlag(AnimationController::STATE_BIT::IDLE);
                anim->ResetFlag(AnimationController::STATE_BIT::WALK);
            }
        }
    }
    else
    {
        // 無効時は停止処理
        velocity_ = {0, 0, 0};
        if(auto se = GetOwner()->GetComponent<Sound>())
        {
            se->Stop("walk");
        }
        if(auto anim = GetOwner()->GetComponent<AnimationController>())
        {
            anim->SetFlag(AnimationController::STATE_BIT::IDLE);
            anim->ResetFlag(AnimationController::STATE_BIT::WALK);
        }
    }
}

//! @brief 移動の有効か無効を設定
//! @param enable 移動可能か
void Move::SetEnable(bool enable)
{
    enabled_ = enable;
    if(!enabled_)
    {
        for(int i = 0; i < WALK_KEY_STATUS::KEY_MAX; i++)
        {
            key_status_[i] = false;
        }
    }
}

//! @brief 最大移動速度(上限)を設定
//! @param speed 速度
void Move::SetMoveSpeed(const float speed)
{
    move_speed_ = speed;
}

//! @brief 回転速度を設定
//! @param speed 回転速度
void Move::SetRotateSpeed(const float speed)
{
    rot_speed_ = speed;
}

//! @brief 最大移動速度(上限)を取得
//! @return 速度
const float Move::GetMoveSpeed() const
{
    return move_speed_;
}

//! @brief 回転速度を取得
//! @return 回転速度
const float Move::GetRotateSpeed() const
{
    return rot_speed_;
}

//! @brief 移動キーを設定
//! @param up 前
//! @param down 後
//! @param left 左
//! @param right 右
void Move::SetKeys(int up, int down, int left, int right)
{
    key_up_    = up;
    key_down_  = down;
    key_left_  = left;
    key_right_ = right;
}

//! @brief GUI表示処理
void Move::GUI()
{
    __super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode(u8"ComponentMove"))
        {
            if(ImGui::Button(u8"削除"))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::DragFloat(u8"加速度", &acceleration_time_, 0.1f, 0.0f, 10.0f);
            ImGui::DragFloat(u8"減速度", &damping_time_, 0.1f, 0.0f, 10.0f);
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//! @brief 歩きの方向を取得(4方向)
//! @return 歩き方向(4方向)
int Move::GetWalkStatus4_Dir() const
{
    return walk_status_;
}

//! @brief 現在(最後)の向きを取得
//! @return 向きベクトル
float3 Move::GetDir() const
{
    return last_dir_;
}

float3 Move::GetVelDir() const
{
    return dir_;
}

//! @brief 指定キーが押されているか取得
//! @param key 取得したいキー
//! @retval true 押されている
//! @retval false 押されていない
bool Move::GetKey(WALK_KEY_STATUS key)
{
    return key_status_[key];
}

//! @brief 移動中かどうか
//! @retval true 移動中
//! @retval false 停止中
bool Move::IsMoving()
{
    return (float)length(velocity_) > FLT_EPSILON;
}

//! @brief プレイヤーの入力設定
void Move::SetInput(bool front, bool back, bool right, bool left)
{
    key_status_[KEY_FRONT] = front;
    key_status_[KEY_BACK]  = back;
    key_status_[KEY_RIGHT] = right;
    key_status_[KEY_LEFT]  = left;
}

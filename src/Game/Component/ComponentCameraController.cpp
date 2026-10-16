//===========================================================================
//! @file ComponentCameraController.cpp
//! @brief カメラ制御コンポーネント
//===========================================================================
#include "ComponentCameraController.h"
#include <System/Component/ComponentSpringArm.h>
#include <System/Component/ComponentModel.h>
#include <Game/Object/Player/Player.h>

//! @brief 初期化
void ComponentCameraController::Init()
{
    __super::Init();

    // プレイヤーが存在する場合
    if(auto player = Scene::Object::Get<Player>())
    {
        // プレイヤーの後ろに置く
        if(auto model = player->GetComponent<ComponentModel>())
        {
            cam_ry_ = model->GetRotationAxisXYZ().y;
        }
    }
}

//! @brief 更新
void ComponentCameraController::Update()
{
    __super::Update();

    // オーナーを取得
    auto owner = GetOwner();

    // ターゲットがいない場合
    if(target_.expired())
    {
        // ターゲットをリセット
        SetTarget(nullptr);
    }

    // カメラが存在している場合
    if(auto camera = Scene::GetCurrentCamera().lock())
    {
        // SpringArmのオブジェクトが自分の場合のみ
        auto cam_owner = camera->GetOwner();
        if(auto arm = cam_owner->GetComponent<ComponentSpringArm>())
        {
            // そのターゲットはオーナーのオブジェクトであることが条件となる
            auto obj = arm->GetSpringArmObject().lock();
            if(obj.get() == owner)
            {
                // オブジェクトコントローラーにターゲットがいる場合
                if(auto target = target_.lock())
                {
                    // プレイヤーを徐々に敵の方に向ける
                    owner->SetRotationToPositionWithLimit(target->GetTranslate(), target_cam_side_speed_);

                    // 角度を360度内に納める
                    auto vec       = target->GetTranslate() - obj->GetTranslate();
                    auto length    = sqrtf(vec.x * vec.x + vec.z * vec.z);
                    float goal_rot = R2D(atan2f(vec.y, length));
                    float next_rot = goal_rot - cam_rx_;

                    while(next_rot >= 180.0f)
                    {
                        next_rot -= 360.0f;
                    }
                    while(next_rot < -180.0f)
                    {
                        next_rot += 360.0f;
                    }
                    float delta_speed = target_cam_side_speed_ * GetDeltaTime60();
                    // カメラを回転
                    if(next_rot >= delta_speed)
                    {
                        cam_rx_ += delta_speed;
                    }
                    else if(next_rot <= -delta_speed)
                    {
                        cam_rx_ -= delta_speed;
                    }
                    else
                    {
                        cam_rx_ = goal_rot;
                    }

                    // ロック時のカメラ回転を設定
                    cam_ry_ = owner->GetRotationAxisXYZ().y;
                    arm->SetSpringArmRotate({cam_rx_, 0, 0});
                }
                else
                {
                    // ターゲットがない場合のマウス操作処理
                    float2 mouse_pos = {GetMouseX(), GetMouseY()};
                    float mouse_ud   = mouse_up_down_;
                    float mouse_lr   = mouse_left_right_;

                    if(IsUseInput())
                    {
                        cam_rx_ += GetMouseMoveY() * (-mouse_ud * 0.01f);
                        if(cam_rx_ > limit_cam_up_)
                        {
                            cam_rx_ = limit_cam_up_;
                        }
                        if(cam_rx_ < limit_cam_down_)
                        {
                            cam_rx_ = limit_cam_down_;
                        }
                        cam_ry_ += GetMouseMoveX() * (mouse_lr * 0.01f);
                        if(cam_ry_ > 360.0f)
                        {
                            cam_ry_ -= 360.0f;
                        }
                        if(cam_ry_ < -360.0f)
                        {
                            cam_ry_ += 360.0f;
                        }

                        if(GetConnecetdPadNum() != -1)
                        {
                            cam_rx_ += mouse_ud * 0.2f * -GetPadInputAnalogRY();
                            if(cam_rx_ > limit_cam_up_)
                            {
                                cam_rx_ = limit_cam_up_;
                            }
                            if(cam_rx_ < limit_cam_down_)
                            {
                                cam_rx_ = limit_cam_down_;
                            }
                            cam_ry_ += mouse_lr * 0.2f * GetPadInputAnalogRX();
                            if(cam_ry_ > 360.0f)
                            {
                                cam_ry_ -= 360.0f;
                            }
                            if(cam_ry_ < -360.0f)
                            {
                                cam_ry_ += 360.0f;
                            }
                        }
                    }

                    // Y軸はオブジェクトそのものの向きを使う
                    owner->SetRotationAxisXYZ({0, cam_ry_, 0});
                    // 上下方向の回転はSpringArmで制御
                    arm->SetSpringArmRotate({cam_rx_, 0, 0});
                }
            }
        }
    }
}

//! @brief ターゲットを設定
//! @param target カメラのターゲット
ComponentCameraControllerPtr ComponentCameraController::SetTarget(ObjectPtr target)
{
    target_ = target;
    return std::dynamic_pointer_cast<ComponentCameraController>(shared_from_this());
}

//! @brief カメラの回転角度を設定
//! @param rot 設定する回転角度
ComponentCameraControllerPtr ComponentCameraController::SetCameraRotate(float3 rot)
{
    cam_rx_ = rot.x;
    cam_ry_ = rot.y;
    return std::dynamic_pointer_cast<ComponentCameraController>(shared_from_this());
}

//! @brief カメラの回転制限を設定
//! @param limit_cam_up カメラの上方向の制限角度
//! @param limit_cam_down カメラの下方向の制限角度
ComponentCameraControllerPtr ComponentCameraController::SetCameraRotateLimit(float limit_cam_up, float limit_cam_down)
{
    limit_cam_up_   = limit_cam_up;
    limit_cam_down_ = limit_cam_down;
    return std::dynamic_pointer_cast<ComponentCameraController>(shared_from_this());
}

//! @brief 操作を設定
//! @param use 操作するか
ComponentCameraControllerPtr ComponentCameraController::SetUseInput(bool use)
{
    use_input_ = use;
    return std::dynamic_pointer_cast<ComponentCameraController>(shared_from_this());
}

//! @brief 操作できるか
//! @retval true できる
//! @retval false できない
bool ComponentCameraController::IsUseInput() const
{
    return use_input_;
}

//! @brief GUI表示処理
void ComponentCameraController::GUI()
{
    __super::GUI();

    // GUI内に出現させる
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode(u8"ComponentCameraController"))
        {
            if(ImGui::Button(u8"削除"))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }

            // カメラの基本情報
            ImGui::Checkbox(u8"カメラ方向にInputを使用", &use_input_);
            ImGui::DragFloat(u8"カメラ左右回転スピード", &mouse_left_right_);
            ImGui::DragFloat(u8"カメラ上下回転スピード", &mouse_up_down_);

            ImGui::DragFloat(u8"見る方向上リミット", &limit_cam_up_);
            ImGui::DragFloat(u8"見る方向下リミット", &limit_cam_down_);

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//! @brief カメラをプレイヤーの背後に設定
void ComponentCameraController::SetCameraToPlayerBehind()
{
    if(auto player = Scene::Object::Get<Player>())
    {
        if(auto model = player->GetComponent<ComponentModel>())
        {
            cam_ry_ = model->GetRotationAxisXYZ().y;
        }
    }
}

//! @brief カメラの回転軸を取得
//! @return カメラの回転軸
float2 ComponentCameraController::GetCameraRotateAxis() const
{
    return float2(cam_rx_, cam_ry_);
}

//! @brief カメラの回転軸を設定
//! @param rx カメラの回転軸
void ComponentCameraController::SetCameraRotateAxis(float2 rx)
{
    cam_rx_ = rx.x;
    cam_ry_ = rx.y;
}

//! @brief カメラの回転軸を加算
//! @param rx 加算するカメラの回転軸
void ComponentCameraController::AddCameraRotateX(float target_rx)
{
    // 角度差分を求める
    float next_rot = target_rx - cam_rx_;

    // -180〜180度の範囲に正規化
    while(next_rot >= 180.0f)
    {
        next_rot -= 360.0f;
    }
    while(next_rot < -180.0f)
    {
        next_rot += 360.0f;
    }

    // カメラを回転
    if(next_rot >= target_cam_side_speed_)
    {
        cam_rx_ += target_cam_side_speed_ * GetDeltaTime60();
    }
    else if(next_rot <= -target_cam_side_speed_)
    {
        cam_rx_ -= target_cam_side_speed_ * GetDeltaTime60();
    }
    else
    {
        cam_rx_ = target_rx;
    }
}

CEREAL_REGISTER_TYPE(ComponentCameraController)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentCameraController)

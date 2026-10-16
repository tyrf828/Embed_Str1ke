//===========================================================================
//! @file Camera.cpp
//! @brief カメラ
//===========================================================================
#include "Camera.h"
#include "Player.h"
#include <Game/Object/Enemy/CloseEnemy/CloseEnemy.h>
#include <System/Component/ComponentCollisionSphere.h>
#include <Game/Component/ComponentCameraController.h>
#include <System/Component/ComponentSpringArm.h>
#include <Game/Object/Player/CameraAimPoint.h>
#include <Game/Object/Enemy/EnemyLockPoint.h>
#include <System/Component/ComponentModel.h>
#include <Game/Component/ComponentHP.h>
#include <System/Component/ComponentCollisionModel.h>
#include <System/Scene.h>
#include <System/Lib.h>

bool Camera::Init()
{
    Super::Init();

    // 追跡カメラ設定
    AddComponent<ComponentCamera>()->SetPositionAndTarget({0, 0, 0}, {0.0f, 0.0f, 1.0f})->SetPerspective(FOV);

    // カメラの視点合わせ用オブジェクト
    Scene::Object::Create<CameraAimPoint>("CameraAimPoint");

    // スプリングアームコンポーネントを追加
    AddComponent<ComponentSpringArm>()
        ->SetSpringArmRotate({0.0f, 0.0f, 0.0f})       // 回転設定
        ->SetSpringArmOffset({0, 0, 0})                // オフセット設定
        ->SetSpringArmLength(SPRING_ARM_MAX_LENGTH_)   // スプリングアームの長さ設定
        ->SetSpringArmObject("CameraAimPoint");        // 追従するオブジェクト設定

    // 衝突コンポーネントを追加
    AddComponent<ComponentCollisionSphere>()
        ->SetCollisionGroup(ComponentCollision::CollisionGroup::CAMERA)
        ->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::ENEMY | (u32)ComponentCollision::CollisionGroup::GROUND)
        ->SetTranslate({0, 0, 0})
        ->SetRadius(2.0f);

    // 初期化
    spring_arm_length_    = SPRING_ARM_MAX_LENGTH_;
    lock_candidate_image_ = LoadGraph("data/Game/GHandle/diamond.png");
    return true;
}

//! @brief 更新
void Camera::Update()
{
    Super::Update();

    Set3DSoundListenerPosAndFrontPos_UpVecY(cast(GetTranslate()), cast(-GetVectorAxisZ()));

    if(camera_shake_time_ > 0.0f)
    {
        // カメラの揺れ時間が残っている場合
        camera_shake_time_ -= GetDeltaTime();
        // カメラを揺らす
        ShakeCamera();
    }
    else
    {
        // カメラの揺れ時間が0以下ならオフセットを戻す
        if(auto spring_arm = GetComponent<ComponentSpringArm>())
        {
            spring_arm->SetSpringArmOffset({0, 0, 0});
        }
    }

    // ステージに当たったフラグを消す
    if(hit_recover_time_ > 0)
    {
        hit_recover_time_ -= GetDeltaTime();
    }
    else
    {
        is_hit_stage_ = false;
    }

    // カメラの前方向ベクトルを取得
    forward_ = GetFront();
    // 正規化
    forward_ = normalize(forward_);
    // カメラの視点合わせ用オブジェクト取得
    auto cam_aim_point = Scene::Object::Get<CameraAimPoint>();
    lock_candidate_.reset();
    if(!cam_aim_point)
    {
        return;
    }
    auto controller = cam_aim_point->GetComponent<ComponentCameraController>();
    if(!controller)
    {
        return;
    }

    // 遮蔽地形チェック
    auto is_hit_ground = [&](const float3& start, const float3& end) -> bool {
        const float dist_target = length(end - start);

        for(auto& obj : Scene::GetCurrentScene()->GetObjectPtrVec())
        {
            if(auto col_model = obj->GetComponent<ComponentCollisionModel>())
            {
                if(static_cast<u32>(col_model->GetCollisionGroup()) & static_cast<u32>(ComponentCollision::CollisionGroup::GROUND))
                {
                    auto stage_model = obj->GetComponent<ComponentModel>();
                    if(!stage_model)
                    {
                        continue;
                    }
                    MV1_COLL_RESULT_POLY hit_poly{};
                    hit_poly = MV1CollCheck_Line(stage_model->GetModelHandle(), -1, cast(start), cast(end));

                    if(hit_poly.HitFlag)
                    {
                        float dh = length(cast(hit_poly.HitPosition) - start);
                        if(dh + FLT_EPSILON < dist_target)
                        {
                            return true;   // 途中で地面にヒット
                        }
                    }
                }
            }
        }
        return false;
    };

    // ロック前から、ボタンを押したときの対象を毎フレーム選ぶ
    if(!target_.lock())
    {
        // ターゲット同士の角度を比較用
        float target_degree_ = D3D11_FLOAT32_MAX;
        // 敵を全てを取得
        for(auto enemy : Scene::Object::GetArray<EnemyBase>())
        {
            // 死亡の敵は除外
            auto hp = enemy->GetComponent<ComponentHP>();
            if(!enemy->GetStatus(Object::StatusBit::Alive) || enemy->GetStatus(Object::StatusBit::Exited) || !hp || hp->GetHP() <= 0)
            {
                continue;
            }
            float3 enemy_pos = enemy->GetVirutalLockPointPos();
            // 長さを取得
            float len = length(enemy->GetTranslate() - cam_aim_point->GetTranslate());
            // 距離以内の敵をターゲット候補に
            if(len < LOCK_LIMIT_DISTANCE_)
            {
                // 距離を比率に変換
                float ratio = 1 - (len / LOCK_LIMIT_DISTANCE_);
                ratio       = std::max(ratio, 0.3f);

                // 角度を計算
                float3 vec = enemy_pos - cam_aim_point->GetTranslate();
                vec        = normalize(vec);
                dot_       = dot(forward_, vec);
                dot_       = (dot_ / (length(forward_) * length(vec)));
                dot_       = std::clamp((float)dot_, -1.0f, 1.0f);
                // 内積を角度に変換
                deg_ = (acosf(dot_)) * RadToDeg;

                // 角度が一定範囲以内
                if(deg_ <= LOCK_DEGREE_ * ratio)
                {
                    // 前のターゲットとの角度を比較
                    if(deg_ < target_degree_)
                    {
                        // 角度で絞ってから遮蔽を調べ、毎フレームの判定回数を抑える
                        if(is_hit_ground(GetTranslate(), enemy_pos))
                        {
                            continue;
                        }
                        // 今のターゲットの角度を代入
                        target_degree_ = deg_;

                        lock_candidate_ = enemy;
                    }
                }
            }
        }
    }

    // キーが押された場合
    if(Input::IsKeyDown(KEY_INPUT_F) || IsPadOn(PAD_ID::PAD_R))
    {
        if(!target_.lock())
        {
            if(auto enemy = lock_candidate_.lock())
            {
                target_ = enemy->CreateEnemyLockPoint();
                controller->SetTarget(target_.lock());
                lock_candidate_.reset();
            }
        }
        else
        {
            // ロック解除
            ResetLockTarget();
        }
    }

    // 一定距離を超えたらロック解除
    if(target_.lock())
    {
        // targetの座標!=0
        if(length(target_.lock()->GetTranslate()) < (float1)FLT_EPSILON)
        {
            return;
        }
        float len = length(target_.lock()->GetTranslate() - cam_aim_point->GetTranslate());
        // 距離を超えた
        if(len > LOCK_LIMIT_DISTANCE_)
        {
            ResetLockTarget();
        }
    }
}

//! @brief 衝突時の処理
void Camera::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    auto other = hit_info.hit_collision_->GetCollisionGroup();

    // 地面と衝突した
    if(static_cast<u32>(other) & static_cast<u32>(ComponentCollision::CollisionGroup::GROUND))
    {
        if(static_cast<u32>(hit_info.collision_->GetCollisionGroup()) & static_cast<u32>(ComponentCollision::CollisionGroup::CAMERA))
        {
            // 衝突フラグを立つ
            is_hit_stage_     = true;
            hit_recover_time_ = HIT_RECOVER_TIME_MAX_;
            // 地面の距離の差分を保存
            distance_ = length(hit_info.hit_position_ - GetTranslate());
        }
    }
    MV1_COLL_RESULT_POLY hit_poly{};
    auto aim_point = Scene::Object::Get<CameraAimPoint>();

    Super::OnHit(hit_info);
}

void Camera::PostUpdate()
{
    // スプリングアームの長さを地面と衝突する距離を減らす
    if(is_hit_stage_)
    {
        spring_arm_length_ -= distance_ - 0.1f;
    }
    else
    {
        // 長さを戻す
        spring_arm_length_ += SPRING_ARM_MAX_LENGTH_ * GetDeltaTime();
    }
    spring_arm_length_ = std::clamp(spring_arm_length_, 0.1f, SPRING_ARM_MAX_LENGTH_);
    // スプリングアームの長さを設定
    if(auto spring_arm = GetComponent<ComponentSpringArm>())
    {
        spring_arm->SetSpringArmLength(spring_arm_length_);
    }
}

//! @brief 描画
void Camera::LateDraw()
{
    Super::LateDraw();
    if(!target_.lock())
    {
        if(auto enemy = lock_candidate_.lock())
        {
            auto hp = enemy->GetComponent<ComponentHP>();
            if(hp->GetHP() > 0)
            {
                float3 position = enemy->GetVirutalLockPointPos();
                if(IsInCameraView(position))
                {
                    VECTOR pos = ConvWorldPosToScreenPos(cast(position));
                    // 距離によって拡大縮小する
                    const float distance = length(position - GetTranslate());
                    const float rate     = std::clamp(1.0f - distance / LOCK_LIMIT_DISTANCE_, 0.8f, 1.0f);
                    DrawRotaGraphF(pos.x, pos.y, 0.05f * rate * SCALE_WINDOW, 0.0, lock_candidate_image_, true);
                }
            }
        }
    }
    if(target_.lock())
    {
        // ロックポイントの2D座標
        VECTOR pos = ConvWorldPosToScreenPos(cast(target_.lock()->GetTranslate()));
        // ロックポイントの座標に円を描画
        DrawCircleAA(pos.x, pos.y, 3, 64, GetColor(255, 255, 255), true);

        if(target_.lock()->HaveOwner())
        {
            // ターゲットの頭座標
            auto head_pos = target_.lock()->GetOwner()->GetComponent<ComponentModel>()->GetNodePosition(6);
            // ターゲットの頭の少し上にずらす
            const float BAR_OFFSET  = 3.0f;
            head_pos.y             += BAR_OFFSET;
            // 頭の2D座標
            VECTOR head_pos_2d = ConvWorldPosToScreenPos(cast(head_pos));

            auto player = Scene::Object::Get<Player>();
            if(!player)
            {
                return;
            }
            // HPバーを距離によって拡大縮小する
            float len              = length(target_.lock()->GetTranslate() - player->GetTranslate());
            float rate             = 1 - (len / LOCK_LIMIT_DISTANCE_);
            rate                   = std::max(rate, 0.6f);
            const float2 OFFSET_2D = {50 * rate, 10 * rate};

            // ターゲットのHPの比率
            auto target_hp = target_.lock()->GetOwner()->GetComponent<ComponentHP>();
            float hp_rate  = (float)target_hp->GetHP() / (float)target_hp->GetMaxHP();

            // HPバーを描画
            DrawBoxAA(head_pos_2d.x - OFFSET_2D.x,
                      head_pos_2d.y - OFFSET_2D.y,
                      head_pos_2d.x - OFFSET_2D.x + (OFFSET_2D.x * hp_rate * 2),
                      head_pos_2d.y + OFFSET_2D.y,
                      GetColor(255, 127, 127),
                      true);
            // HPバーの枠を描画
            DrawBoxAA(head_pos_2d.x - OFFSET_2D.x,
                      head_pos_2d.y - OFFSET_2D.y,
                      head_pos_2d.x + OFFSET_2D.x,
                      head_pos_2d.y + OFFSET_2D.y,
                      GetColor(255, 255, 255),
                      false);
        }
    }
}

//! @brief 終了
void Camera::Exit()
{
    DeleteGraph(lock_candidate_image_);

    lock_candidate_.reset();
    Super::Exit();
}

//! @brief カメラの揺れ処理
void Camera::ShakeCamera()
{
    if(auto spring_arm = GetComponent<ComponentSpringArm>())
    {
        spring_arm->SetSpringArmOffset(
            {GetRandomF(camera_shake_power_) - camera_shake_power_ * 0.5f, GetRandomF(camera_shake_power_) - camera_shake_power_ * 0.5f, 0});
    }
}

//! @brief カメラの揺れ処理
void Camera::ShakeCamera(float sec, float power)
{
    // カメラの揺れパワーを設定
    camera_shake_power_ = power;
    // カメラの揺れ時間を設定
    camera_shake_time_ = sec;
}

//! @brief カメラの前方向ベクトルを取得
//! @return カメラの前方ベクトル
float3 Camera::GetFront() const
{
    matrix mat_world = GetWorldMatrix();
    float3 front_dir = mat_world.axisZ();
    return front_dir;
}

//! @brief カメラの平面方向ベクトルを取得
//! @return カメラの平面上の前方ベクトル
float3 Camera::GetPlaneFront() const
{
    float3 front_dir = GetWorldMatrix().axisZ();
    front_dir.y      = 0.0f;
    return normalize(front_dir);
}

//! @brief カメラの右方向ベクトルを取得
//! @return カメラの右方向ベクトル
float3 Camera::GetRight() const
{
    matrix mat_world = GetWorldMatrix();
    float3 right_dir = mat_world.axisX();
    return right_dir;
}

//! @brief カメラの平面右方向ベクトルを取得
//! @return カメラの平面上の右方向ベクトル
float3 Camera::GetPlaneRight() const
{
    float3 right_dir = GetWorldMatrix().axisX();
    right_dir.y      = 0.0f;
    return normalize(right_dir);
}

//! @brief カメラがロックしているか
//! @retval true している
//! @retval false していない
bool Camera::IsCameraLock() const
{
    return target_.lock().get();
}

//! @brief カメラに映っているか
//! @retval true 映っている
//! @retval false 映っていない
bool Camera::IsInCameraView(const float3& position) const
{
    // カメラコンポーネントを取得
    auto camera = GetComponent<ComponentCamera>();
    if(!camera)
    {
        return false;
    }
    matrix mat_view    = camera->GetViewMatrix();
    matrix mat_proj    = camera->GetProjMatrix();
    auto mat_view_proj = mul(mat_view, mat_proj);
    float4 pos_in_vp   = mul(float4(position, 1.0f), mat_view_proj);
    pos_in_vp.xyz      = pos_in_vp.xyz / pos_in_vp.w;
    if(pos_in_vp.x < -1.0f || pos_in_vp.x > 1.0f || pos_in_vp.y < -1.0f || pos_in_vp.y > 1.0f || pos_in_vp.z < 0.0f || pos_in_vp.z > 1.0f)
    {
        return false;
    }

    return true;   // カメラに映っている
}

//! @brief ロック対象リセット
void Camera::ResetLockTarget()
{
    if(target_.lock())
    {
        // 現在のロックポイントを削除
        target_.lock()->GetOwner()->ClearEnemyLockPoint();
        // ロック解除
        target_.reset();
        // カメラの視点合わせ用オブジェクト取得
        if(auto cam_aim_point = Scene::Object::Get<CameraAimPoint>())
        {
            if(auto controller = cam_aim_point->GetComponent<ComponentCameraController>())
            {
                controller->SetTarget(nullptr);
            }
        }
    }
}

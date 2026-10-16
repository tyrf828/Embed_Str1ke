//===========================================================================
//! @file Player.cpp
//! @brief プレイヤー
//===========================================================================
#include "Player.h"
#include <Game/Component/Player/Attack.h>
#include <Game/Component/Player/Attack2.h>
#include <Game/Component/Player/Attack3.h>
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Component/Player/Move.h>
#include <Game/Component/Player/AnimationController.h>
#include <Game/Component/ComponentCameraController.h>
#include <Game/Component/Player/Stamina.h>
#include <Game/Component/ComponentHP.h>
#include <Game/Component/Player/Regenerate.h>
#include <Game/Component/Sound.h>
#include <Game/Component/Player/UltimateAttack/UltimateAttack.h>
#include <Game/Component/Player/CombineAttack.h>
#include <Game/Component/Player/UltimateGauge.h>
#include <Game/Component/Enemy/AttackParam.h>
#include <Game/Component/Player/UltimateAttack/UltimateBase.h>
#include <Game/Object/BlackScreen.h>
#include <System/Utils/IniFileLib.h>
#include <Game/Scene/SceneRetry.h>
#include <Game/Object/ScanWave.h>

//! @brief 初期化
//! @return 初期化済み
bool Player::Init()
{
    Super::Init();

    SetScaleAxisXYZ(float3{1.3f, 1.3f, 1.3f});

    // 移動コンポーネントの追加と設定
    auto move = AddComponent<Move>();
    move->SetMoveSpeed(MOVE_SPEED_);

    AddComponent<AnimationController>();

    // モデルコンポーネント
    auto anim = AddComponent<ComponentModel>("data/Game/Model/Player/Arissa.mv1");

    // 衝突判定コンポーネント
    auto col = AddComponent<ComponentCollisionCapsule>();
    col->SetRadius(2.3f)
        ->SetHeight(15.0f)
        ->SetMass(10.0f)
        ->SetCollisionGroup(ComponentCollision::CollisionGroup::PLAYER)
        ->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::GROUND | (u32)ComponentCollision::CollisionGroup::ENEMY |
                               (u32)ComponentCollision::CollisionGroup::ENEMY_WEAPON | (u32)ComponentCollision::CollisionGroup::ENEMY_WEAPON2 |
                               (u32)ComponentCollision::CollisionGroup::ITEM | (u32)ComponentCollision::CollisionGroup::STAGE_TRIGGER)
        ->UseGravity();

    // HPをファイルから読み込み
    IniFileLib ini("Game/PlayerData/PlayerData.ini");
    max_hp_      = ini.GetInt("Player", "MaxHP", 1000);
    const int hp = ini.GetInt("Player", "HP", max_hp_);
    // HPコンポーネントの追加
    AddComponent<ComponentHP>()->SetMaxHP(max_hp_)->SetHP(hp);

    // スタミナコンポーネントの追加
    AddComponent<Stamina>();
    const int utimate = ini.GetInt("Player", "Ultimate", 0);
    AddComponent<UltimateGauge>()->SetEnergy(utimate);

    AddComponent<Sound>()->Load({
        {"walk",             "data/Game/SE/Player/革靴で走る.mp3",              128.0f, 0.5f},
        {"run",              "data/Game/SE/Player/革靴で速く走る.mp3",          128.0f, 0.7f},
        {"attack1",          "data/Game/SE/Player/「とう！」.mp3",              128.0f, 0.7f},
        {"attack2",          "data/Game/SE/Player/「たあ！」.mp3",              128.0f, 0.7f},
        {"attack3",          "data/Game/SE/Player/「くらえ！」.mp3",            128.0f, 0.7f},
        {"regen",            "data/Game/SE/Player/飲む.mp3",                    128.0f, 0.7f},
        {"ultimate start",   "data/Game/SE/Player/「紫電一閃！」.mp3",          256.0f, 2.0f},
        {"ultimate range",   "data/Game/SE/Player/ビームガン銃撃戦.mp3",        256.0f, 1.0f},
        {"ultimate thunder", "data/Game/SE/Player/maou_se_magic_thunder04.mp3", 256.0f, 1.3f},
        {"hit",              "data/Game/SE/Player/「ぐっ！」.mp3",              256.0f, 1.0f},
        {"hit heavy",        "data/Game/SE/Player/「ぐああっ！」.mp3",          256.0f, 1.0f},
        {"death",            "data/Game/SE/Player/「ぐああーーっ！」.mp3",      256.0f, 1.0f},
    });

    // 初期化
    action_enable_   = true;
    is_hit_          = false;
    const int bottle = ini.GetInt("Player", "Bottle", HP_BOTTLE_MAX_);
    hp_bottle_       = bottle;

    if(!Scene::Object::Get<ScanWave>())
    {
        Scene::Object::Create<ScanWave>("ScanWave", true);
    }

    return true;
}

//! @brief 更新
void Player::Update()
{
    Super::Update();

    auto model = GetComponent<ComponentModel>();
    auto move  = GetComponent<Move>();
    auto anim  = GetComponent<AnimationController>();

    // 被弾処理
    HitProcess();
    if(is_hit_)
    {
        // 当たられたら下の処理を通らないように
        return;
    }

    StopRunCheck();
    // 行動可能かどうか
    if(!action_enable_)
    {
        is_run_stop_ = true;
        StopRunProcess();
        return;
    }

    // スキャン
    if(Input::IsKeyDown(KEY_INPUT_Q))
    {
        if(auto scan = Scene::Object::Get<ScanWave>())
        {
            scan->Start(GetTranslate());
        }
    }

    const float INPUT_THRESHOLD = 5.0f;
    if(GetConnecetdPadNum() != -1)
    {
        if(is_run_stop_ && key_frame_time_ < RUN_KEY_FRAME_ && !is_run_ && key_frame_time_ > INPUT_THRESHOLD)
        {
            if(auto scan = Scene::Object::Get<ScanWave>())
            {
                scan->Start(GetTranslate());
            }
        }
    }

    // 移動処理
    if(move)
    {
        move->SetInput(Input::IsKey(KEY_INPUT_W) || IsPadRepeat(PAD_ID::PAD_UP),
                       Input::IsKey(KEY_INPUT_S) || IsPadRepeat(PAD_ID::PAD_DOWN),
                       Input::IsKey(KEY_INPUT_D) || IsPadRepeat(PAD_ID::PAD_RIGHT),
                       Input::IsKey(KEY_INPUT_A) || IsPadRepeat(PAD_ID::PAD_LEFT));
    }

    // 必殺技攻撃
    if(IsKeyDown(KEY_INPUT_V) || IsPadOn(PAD_ID::PAD_Y))
    {
        ResetRunState();
        if(auto ultimate = GetComponent<UltimateGauge>())
        {
            if(ultimate->GetEnergy() >= ultimate->GetUltimateCost())
            {
                AddComponent<UltimateAttack>();
                ultimate->SetEnergy(0);
                SetActionEnable(false);
            }
        }
        return;
    }

    // HPが0以下の場合、死んだ処理を行う
    if(auto hp = GetComponent<ComponentHP>())
    {
        // 死んだときの処理
        if(hp->GetHP() <= 0)
        {
            DeathProcess();
            if(anim)
            {
                // 死亡アニメーション再生
                anim->SetFlag(AnimationController::STATE_BIT::DEATH);
            }

            if(move)
            {
                // 移動禁止
                move->SetEnable(false);

                if(auto cam = GetComponent<ComponentCameraController>())
                {
                    // カメラ回転禁止
                    cam->SetCameraRotate(float3{0, 0, 0});
                    cam->SetUseInput(false);
                }
            }

            if(auto col = GetComponent<ComponentCollisionCapsule>())
            {
                // 当たり判定を消去
                RemoveComponent(col);
            }

            // 死んだら下の処理を通らないように
            return;
        }
    }

    // 近接軽攻撃
    if(IsKeyDown(KEY_INPUT_C) || IsMouseDown(MOUSE_INPUT_LEFT) || IsPadOn(PAD_ID::PAD_X))
    {
        if(!GetComponent<Attack>())
        {
            if(auto sta = GetComponent<Stamina>())
            {
                if(sta->GetStamina() > 0)
                {
                    // 近接攻撃中
                    SetActionEnable(false);
                    // スタミナを減らす
                    sta->SetStaminaCoolTime();
                    sta->SubStamina(ATTACK_STAMINA_);
                    // 攻撃コンポーネントを追加
                    AddComponent<Attack>();
                }
            }
        }
    }

    // 吸着攻撃
    if(IsMouseDown(MOUSE_INPUT_RIGHT) || IsPadOn(PAD_ID::PAD_B))
    {
        if(!GetComponent<CombineAttack>())
        {
            if(auto sta = GetComponent<Stamina>())
            {
                if(sta->GetStamina() > 0)
                {
                    // 攻撃コンポーネントを追加
                    AddComponent<CombineAttack>();
                    // スタミナを減らす
                    sta->SetStaminaCoolTime();
                    sta->SubStamina(static_cast<int>(ATTACK_STAMINA_ * 1.5f));
                    SetActionEnable(false);
                }
            }
        }
    }

    // キーが押している処理
    // 走るか回避に入る処理
    if(Input::IsKey(KEY_INPUT_SPACE) || IsPadRepeat(PAD_ID::PAD_A))
    {
        // 他のアクションコンポーネントが実行中でないことを確認
        if(!CheckOtherActionComponent())
        {
            // 走るに必要なフレームを溜まったら走る
            key_frame_time_ += GetDeltaTime60();
            key_frame_time_  = std::clamp(key_frame_time_, 0.0f, RUN_KEY_FRAME_);
            if(key_frame_time_ >= RUN_KEY_FRAME_)
            {
                is_run_ = true;
            }
        }
    }
    // キーが離された場合の処理
    if(Input::IsKeyUp(KEY_INPUT_SPACE) || IsPadUp(PAD_ID::PAD_A))
    {
        is_run_stop_ = true;
    }

    if(move && anim)
    {
        if(auto sta = GetComponent<Stamina>())
        {
            //! スタミナがある場合かつ移動している場合
            if(sta->GetStamina() > 0 && is_run_ && move->IsMoving())
            {
                // アニメーションを再生
                anim->SetFlag(AnimationController::STATE_BIT::RUN);
                //! スピードを上げる
                move->SetMoveSpeed(MOVE_SPEED_ * 1.5f);

                // スタミナを消費
                sta->SetStaminaCoolTime();
                sta->SubStamina(RUN_STAMINA_COST_);

                if(auto se = GetComponent<Sound>())
                {
                    // 音を再生
                    se->PlayNoSame("run");
                }
            }
            else
            {
                StopRunProcess();
            }
        }
    }

    // 回復処理
    if(Input::IsKeyDown(KEY_INPUT_R) || IsPadOn(PAD_ID::PAD_L))
    {
        // 他のアクションコンポーネントが実行中でないことを確認
        if(!CheckOtherActionComponent())
        {
            // HPアイテムが残っている場合
            if(hp_bottle_ > 0)
            {
                if(!GetComponent<Regenerate>())
                {
                    // 回復コンポーネントを追加
                    AddComponent<Regenerate>();
                    // HPアイテムを消費
                    hp_bottle_--;
                    SetActionEnable(false);
                }
            }
        }
    }
}

void Player::PostUpdate()
{
    Super::PostUpdate();
}

//! @brief 衝突時の処理
void Player::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    // アニメーションコントローラーを取得
    auto anim = GetComponent<AnimationController>();

    // 地面と衝突した場合
    if(static_cast<u32>(hit_info.hit_collision_->GetCollisionGroup()) & static_cast<u32>(ComponentCollision::CollisionGroup::GROUND))
    {
        // 地面に接触している状態を設定
        is_ground_ = true;
    }

    if(static_cast<u32>(hit_info.hit_collision_->GetCollisionGroup()) & static_cast<u32>(ComponentCollision::CollisionGroup::ENEMY_WEAPON2))
    {
        // 当たられたら下の処理をしない
        if(is_hit_)
        {
            return;
        }
        if(auto e_atk = hit_info.hit_collision_->GetOwner()->GetComponent<AttackParam>())
        {
            if(auto hp = GetComponent<ComponentHP>())
            {
                hp->SubHP(e_atk->GetAttack());
                IniFileLib ini("Game/PlayerData/PlayerData.ini");
                ini.WriteInt("Player", "HP", hp->GetHP());
            }
        }
        // ヒット処理フラグを立てる
        is_hit_ = true;
        if(auto se = GetComponent<Sound>())
        {
            if(auto hp = GetComponent<ComponentHP>())
            {
                // HPが0以下なら死亡音、0より大きければヒット音を再生
                if(hp->GetHP() <= 0)
                {
                    se->PlayNoSame("death");
                }
                else
                {
                    se->PlayNoSame("hit heavy");
                }
            }
        }
        if(anim)
        {
            // アニメーションを再生
            anim->SetFlag(AnimationController::STATE_BIT::BIG_HIT);
        }
        if(auto ctrl = GetComponent<Move>())
        {
            // 移動を無効化
            ctrl->SetEnable(false);
        }
        if(auto model = GetComponent<ComponentModel>())
        {
            auto hit_owner = hit_info.hit_collision_->GetOwner();
            auto vec       = GetTranslate() - hit_owner->GetTranslate();
            vec            = normalize(vec);
            // ノックバックの力を設定
            hit_knockback_ = vec;
        }
        // 地面から離れたフラグを消す
        is_ground_ = false;
    }

    // 敵の武器と衝突した場合
    if(static_cast<u32>(hit_info.hit_collision_->GetCollisionGroup()) & static_cast<u32>(ComponentCollision::CollisionGroup::ENEMY_WEAPON))
    {
        // 当たられたら下の処理をしない
        if(is_hit_)
        {
            return;
        }

        // ヒット処理フラグを立てる
        is_hit_ = true;
        if(auto e_atk = hit_info.hit_collision_->GetOwner()->GetComponent<AttackParam>())
        {
            if(auto hp = GetComponent<ComponentHP>())
            {
                hp->SubHP(e_atk->GetAttack());
                IniFileLib ini("Game/PlayerData/PlayerData.ini");
                ini.WriteInt("Player", "HP", hp->GetHP());

                if(auto se = GetComponent<Sound>())
                {
                    // HPが0以下なら死亡音、0より大きければヒット音を再生
                    if(hp->GetHP() <= 0)
                    {
                        se->PlayNoSame("death");
                    }
                    else
                    {
                        se->PlayNoSame("hit");
                    }
                }
            }
        }
        if(anim)
        {
            // アニメーションを再生
            anim->SetFlag(AnimationController::STATE_BIT::SMALL_HIT);
        }
        if(auto ctrl = GetComponent<Move>())
        {
            // 移動を無効化
            ctrl->SetEnable(false);
        }
    }

    Super::OnHit(hit_info);
}

//! @brief プレイヤーの前方向をの平面ベクトルを取得
inline float3 Player::GetPlaneFront()
{
    // 前方向ベクトル
    float3 front = {0.0f, 0.0f, 1.0f};
    // 回転行列を取得
    MATRIX mat = MGetRotY(GetRotationAxisXYZ().y * DegToRad);

    VECTOR base = cast(front);
    // ベクトルを変換
    VECTOR change = VTransform(base, mat);

    // 前方向の平面ベクトル
    float3 front_dir = cast(change);
    return front_dir;
}

//! @brief モデルの前方向平面ベクトルを取得
inline float3 Player::GetModelPlaneFront(ComponentModelPtr model)
{
    // 前方向ベクトル
    float3 front = {0.0f, 0.0f, 1.0f};
    // 回転行列を取得
    MATRIX mat = MGetRotY(model->GetRotationAxisXYZ().y * DegToRad);

    VECTOR base = cast(front);
    // ベクトルを変換
    VECTOR change = VTransform(base, mat);

    // 前方向の平面ベクトル
    float3 front_dir = cast(change);
    return front_dir;
}

//! @brief ヒット状態かを取得
bool Player::IsHit()
{
    return is_hit_;
}

//! @brief 行動可能かどうかを設定
//! @return 行動可能かどうか
void Player::SetActionEnable(bool enable)
{
    action_enable_ = enable;

    if(auto move = GetComponent<Move>())
    {
        move->SetInput();
        move->SetEnable(enable);
    }
}

//! @brief 回復しているかどうか
bool Player::IsRegenerateHP() const
{
    return is_regen_hp_;
}

void Player::SetRegenerateHP(bool regen)
{
    is_regen_hp_ = regen;
}

//! @brief HPボトルの残量を取得
int Player::GetHPBottleAmount() const
{
    return hp_bottle_;
}

//! @brief HPボトルの残量を設定
//! @param value 設定するHPボトルの数
void Player::SetHPBottleAmount(int value)
{
    hp_bottle_ = value;
    IniFileLib ini("Game/PlayerData/PlayerData.ini");
    ini.WriteInt("Player", "Bottle", hp_bottle_);
}

//! @brief 最大HPボトル数を取得
int Player::GetHPBottleMax() const
{
    return HP_BOTTLE_MAX_;
}

//! @brief プレイヤーとオブジェクトとの角度を計算
//! @param obj_ptr オブジェクト
float Player::PlayerToObjectDegree(ObjectPtr obj_ptr)
{
    // プレイヤーのモデルコンポーネントを取得
    auto model = GetComponent<ComponentModel>();
    if(!model)
    {
        return -1;
    }
    // オブジェクトの位置とプレイヤーの位置の距離
    float3 vec = obj_ptr->GetTranslate() - GetTranslate();
    // 正規化
    vec = normalize(vec);
    // 内積を計算
    float dot_ = dot(model->GetPlaneFront(), vec);
    // 正規化した内積を計算
    dot_ = (dot_ / (length(model->GetPlaneFront()) * length(vec)));
    // 範囲制限
    dot_ = std::clamp((float)dot_, -1.0f, 1.0f);
    // 角度に変換する
    float deg = (acosf(dot_)) * RadToDeg;
    return deg;
}

//! @brief 他のアクションコンポーネントのチェック
//! @retval true 他のアクションコンポーネントがある
//! @retval false 他のアクションコンポーネントがいない
bool Player::CheckOtherActionComponent()
{
    auto attack  = GetComponent<Attack>();
    auto attack2 = GetComponent<Attack2>();
    auto attack3 = GetComponent<Attack3>();

    auto regen = GetComponent<Regenerate>();

    // 他のアクションが存在すればtrueを返す
    if(attack || attack2 || attack3 || regen)
    {
        return true;
    }
    return false;
}

void Player::ResetRunState()
{
    is_run_         = false;
    is_run_stop_    = false;
    key_frame_time_ = 0.0f;
    if(auto move = GetComponent<Move>())
    {
        // 移動速度を元に戻す
        move->SetMoveSpeed(MOVE_SPEED_);
    }
    if(auto se = GetComponent<Sound>())
    {
        // 音を停止
        se->Stop("run");
    }
}

//! @brief 被弾状態の処理
void Player::HitProcess()
{
    auto move = GetComponent<Move>();
    auto anim = GetComponent<AnimationController>();
    if(!move || !anim)
    {
        return;
    }
    // 当たられた処理
    if(is_hit_)
    {
        if(move)
        {
            // 移動禁止
            move->SetEnable(false);
        }
        if(anim)
        {
            // 当たられたアニメーション再生終了後
            if(!anim->GetFlag(AnimationController::STATE_BIT::SMALL_HIT) && !anim->GetFlag(AnimationController::STATE_BIT::BIG_HIT) &&
               !anim->GetFlag(AnimationController::STATE_BIT::STAND_UP))
            {
                if(auto ctrl = GetComponent<Move>())
                {
                    // 移動可能
                    ctrl->SetEnable(true);
                    // 当たられた可能になる
                    is_hit_ = false;
                }
            }
            // 当たられたアニメーション再生中
            if(anim->GetFlag(AnimationController::STATE_BIT::BIG_HIT))
            {
                if(!is_ground_)
                {
                    // ノックバック
                    AddTranslate({hit_knockback_.x, 0.6f, hit_knockback_.z});
                }
            }
        }
    }
}

//! @brief 死亡状態の処理
void Player::DeathProcess()
{
    if(auto hp = GetComponent<ComponentHP>())
    {
        if(hp->GetHP() <= 0)
        {
            // 画面をフェードイン
            auto death_screen = Scene::Object::Get<BlackScreen>("DeathScreen");
            if(!death_screen)
            {
                death_screen = Scene::Object::Create<BlackScreen>("DeathScreen");
                death_screen->SetFadeTime(3.0f)->StartFadeIn(false);
            }
            else
            {
                // リトライシーンへ移動
                if(death_screen->IsFadeComplete())
                {
                    Scene::Change(Scene::GetScene<SceneRetry>());
                }
            }
            return;
        }
    }
}

void Player::StopRunCheck()
{
    if(is_run_stop_)
    {
        key_frame_time_ -= GetDeltaTime60();
        key_frame_time_  = std::clamp(key_frame_time_, 0.0f, RUN_KEY_FRAME_);
        if(key_frame_time_ <= 0.0f)
        {
            is_run_      = false;
            is_run_stop_ = false;
        }
    }
}

//! @brief 走りから歩きに戻る
void Player::StopRunProcess()
{
    if(auto anim = GetComponent<AnimationController>())
    {
        // 走るアニメーションをリセット
        anim->ResetFlag(AnimationController::STATE_BIT::RUN);
    }
    if(auto move = GetComponent<Move>())
    {
        // 移動速度を元に戻す
        move->SetMoveSpeed(MOVE_SPEED_);
    }
    if(auto se = GetComponent<Sound>())
    {
        // 音を停止
        se->Stop("run");
    }
}

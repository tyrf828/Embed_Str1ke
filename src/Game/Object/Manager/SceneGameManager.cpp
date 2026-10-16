//===========================================================================
//! @file SceneGameManager.cpp
//! @brief ゲームシーンマネージャー
//===========================================================================
#include "SceneGameManager.h"
#include <Game/Component/DialogueVoice.h>
#include <Game/Object/Manager/SpawnManager.h>
#include <Game/Object/StageTrigger.h>
#include <Game/Scene/SceneCutScene.h>
#include <Game/Object/BlackScreen.h>
#include <Game/Component/UI/AutoDrawText.h>
#include <Game/Object/Player/Player.h>
#include <Game/Component/ComponentHP.h>
#include <System/Component/ComponentCollisionCapsule.h>
#include <System/Component/ComponentModel.h>
#include <Game/Object/Player/Camera.h>
#include <Game/Object/Player/GameUI.h>
#include <Game/Object/Player/CameraAimPoint.h>
#include <Game/Component/ComponentCameraController.h>
#include <Game/Object/Manager/EnemyActionManager.h>
#include <Game/Object/Enemy/EnemyBase.h>
#include <Game/Component/Enemy/ActionBase/ActionAttackBase.h>
#include <Game/Object/Weapon/Stick.h>
#include <Game/Component/Weapon/AttachWeaponParts.h>
#include <Game/Component/Weapon/AttachWeapon.h>

//! @brief 初期化
//! @return 初期化済み
bool SceneGameManager::Init()
{
    Super::Init();

    quest_timer_      = QUEST_CLEAR_TIME;
    text_box_ghandle_ = LoadGraph("data/Game/GHandle/box_blue.png");

    return true;
}

//! @brief 更新
void SceneGameManager::Update()
{
    Super::Update();

    // クエストタイマー更新
    quest_timer_ -= GetDeltaTime();

    quest_timer_ = std::max(0.0f, quest_timer_);
    if(quest_timer_ > 0.0f)
    {
        return;
    }

    // ボスガイド更新
    BossNavigationGudie();

    if(is_boss_guide_finish_)
    {
        if(!boss_show_camera_ && !boss_show_timer_active_)
        {
            Scene::Object::Create<BlackScreen>("BlackScreen")->SetFadeTime(0.2f)->StartFadeOut(true);
            boss_show_camera_ = Scene::Object::Create<Object>("BossShowCamera");
            boss_show_camera_->SetTranslate(float3{-660.0f, 170.0f, -118.0f})->SetRotationAxisXYZ(float3{0.0f, -30.0f, 0.0f});
            boss_show_camera_->AddComponent<ComponentCamera>()->SetCurrentCamera()->SetPerspective(68.0f);
            boss_show_elapsed_      = 0.0f;
            boss_show_timer_active_ = true;
            if(auto ui = Scene::Object::Get<GameUI>())
            {
                ui->UseDrawUI(false);
            }
            if(player_)
            {
                player_->SetActionEnable(false);
            }
            if(auto camera_aim = Scene::Object::Get<CameraAimPoint>())
            {
                if(auto cam_control = camera_aim->GetComponent<ComponentCameraController>())
                {
                    cam_control->SetUseInput(false);
                    float2 axis = cam_control->GetCameraRotateAxis();
                    cam_control->SetCameraRotateAxis(float2{-35.0f, axis.y});
                }
            }
        }
    }

    if(boss_show_timer_active_)
    {
        boss_show_elapsed_ += GetDeltaTime();

        const float BOSS_SHOW_DURATION = 1.5f;
        boss_show_elapsed_             = std::min(boss_show_elapsed_, BOSS_SHOW_DURATION);
        if(boss_show_elapsed_ >= BOSS_SHOW_DURATION)
        {
            if(boss_show_camera_)
            {
                Scene::Object::Create<BlackScreen>("BlackScreen")->SetFadeTime(0.2f)->StartFadeOut(true);
                Scene::Object::Release(boss_show_camera_);
                boss_show_camera_.reset();
                if(auto camera = Scene::Object::Get<Camera>())
                {
                    auto cmp_camera = camera->GetComponent<ComponentCamera>();
                    if(cmp_camera)
                    {
                        cmp_camera->SetCurrentCamera();
                    }
                }
                if(auto ui = Scene::Object::Get<GameUI>())
                {
                    ui->UseDrawUI();
                }
                if(auto camera_aim = Scene::Object::Get<CameraAimPoint>())
                {
                    if(auto cam_control = camera_aim->GetComponent<ComponentCameraController>())
                    {
                        cam_control->SetUseInput(true);
                    }
                }
                if(player_)
                {
                    player_->SetActionEnable(true);
                }
            }
        }
        else
        {
            for(auto enemy : Scene::Object::GetArray<EnemyBase>())
            {
                auto action = enemy->GetComponent<EnemyActionBase>();
                if(!action)
                {
                    continue;
                }

                // 既に攻撃中の敵をキャンセルする
                if(dynamic_cast<ActionAttackBase*>(action->GetCurrentAction().get()))
                {
                    action->SetFindPlayer();
                }
            }
        }
    }

    // スポナーのクリア処理
    for(auto& spawner : spawners_)
    {
        spawner->StartClearObjects();
    }
    // ボス壁の削除
    if(boss_wall_)
    {
        Scene::Object::Release(boss_wall_);
    }
    // ボスエリア侵入判定の作成
    if(!enter_boss_checker)
    {
        enter_boss_checker = Scene::Object::Create<StageTrigger>("EnterBossChecker");
        enter_boss_checker->SetTranslate(float3{-844.0f, 113.0f, -53.0f});
        enter_boss_checker->SetSize(float3{50.0f, 30.0f, 30.0f});
        return;
    }

    // ボスエリア侵入チェック
    if(!enter_boss_checker->IsHit())
    {
        return;
    }
    // フェードアウトしてシーン遷移
    auto black_screen = Scene::Object::Get<BlackScreen>();
    if(!black_screen)
    {
        black_screen = Scene::Object::Create<BlackScreen>("BlackScreen");
        black_screen->SetFadeTime(2.0f)->StartFadeIn(false);
    }
    else
    {
        // フェード完了でシーン遷移
        if(black_screen->IsFadeComplete())
        {
            auto stick = Scene::Object::Get<Stick>();
            if(stick)
            {
                stick->DontDestroyOnLoad();
            }
            auto attach = stick->GetComponent<AttachWeaponParts>();
            // 全部削除しないようにする
            for(auto& part : attach->GetParts())
            {
                part->DontDestroyOnLoad();
                auto attach_weapon = part->GetComponent<AttachWeapon>();
                attach_weapon->GetWeaponJoint()->DontDestroyOnLoad();
            }
            Scene::Change(Scene::GetScene<SceneCutScene>());
        }
    }
}

//! @brief 描画
void SceneGameManager::Draw()
{
    Super::Draw();
}

//! @brief 遅延描画
void SceneGameManager::LateDraw()
{
    Super::LateDraw();
    if(quest_timer_ > 0.0f)
    {
        return;
    }
    // ナビゲーション矢印描画
    DrawNaviArrow();
    // テキストボックス描画
    int size_x, size_y;
    GetGraphSize(text_box_ghandle_, &size_x, &size_y);
    const float SCALE = 0.6f * SCALE_WINDOW;
    DrawRotaGraphF(WINDOW_W - size_x * 0.5f * SCALE, WINDOW_H * 0.3f + (size_y * 0.5f * SCALE), SCALE * text_box_scale_, 0.0f, text_box_ghandle_, TRUE);
}

//! @brief 終了
void SceneGameManager::Exit()
{
    if(dialogue_voice_)
    {
        dialogue_voice_->Stop();
        RemoveComponent(dialogue_voice_);
        dialogue_voice_.reset();
    }
    Super::Exit();
    // リソース解放
    Scene::Object::Release(enter_boss_checker);
    enter_boss_checker.reset();
    for(auto& spawner : spawners_)
    {
        Scene::Object::Release(spawner);
    }
    spawners_.clear();
    Scene::Object::Release(boss_wall_);
    Scene::Object::Release(arrow_);
    DeleteGraph(text_box_ghandle_);
    RemoveComponent(text_);
    boss_wall_.reset();
    text_.reset();
    arrow_.reset();
}

//! @brief ボスナビゲーションガイド
void SceneGameManager::BossNavigationGudie()
{
    // テキストボックスの開閉アニメーション
    if(!is_boss_guide_close_)
    {
        text_box_scale_ += GetDeltaTime() * 2.0f;
        text_box_scale_  = std::min(text_box_scale_, 1.0f);
    }
    else
    {
        text_box_scale_ -= GetDeltaTime() * 2.0f;
        text_box_scale_  = std::max(0.0f, text_box_scale_);
        if(text_box_scale_ <= 0.0f)
        {
            is_boss_guide_finish_ = true;
        }
    }

    if(text_box_scale_ < 1.0f)
    {
        return;
    }

    // テキスト表示コンポーネントの作成
    if(!text_)
    {
        text_        = AddComponent<AutoDrawText>();
        guide_texts_ = text_->LoadTextFile("data/Game/TextData/SceneGameClear.txt");
        text_->SetText(guide_texts_[current_guide_]);
        dialogue_voice_ = AddComponent<DialogueVoice>();
        dialogue_voice_->SetSequence("SceneGameClear", current_guide_);
        text_->SetFontSize(16);
        text_->SetDrawBox(float2{WINDOW_W * 0.66, WINDOW_H * 0.34f}, float2{WINDOW_W * 0.98, WINDOW_H * 0.41f});
    }

    // テキストの更新
    if(!text_->IsDrawComplete())
    {
        text_->StartDraw(false);
        dialogue_voice_->Play();
    }
    else
    {
        text_elapsed_time_ += GetDeltaTime();
        // 一定時間経過で次のテキストへ
        if(text_elapsed_time_ >= TEXT_STAY_TIME_ && (!dialogue_voice_ || !dialogue_voice_->IsPlaying()))
        {
            text_elapsed_time_ = 0.0f;
            current_guide_++;
            if(current_guide_ < guide_texts_.size())
            {
                text_->SetText(guide_texts_[current_guide_]);
                dialogue_voice_->PlayMessage(current_guide_);
                text_->StartDraw(true);
            }
            else
            // 全てのテキスト表示完了で閉じる
            {
                is_boss_guide_close_ = true;
                dialogue_voice_->Stop();
                RemoveComponent(dialogue_voice_);
                dialogue_voice_.reset();
                RemoveComponent(text_);
                text_.reset();
            }
        }
    }
}

//! @brief ナビゲーション矢印描画
void SceneGameManager::DrawNaviArrow()
{
    if(!player_)
    {
        return;
    }
    if(!is_boss_guide_finish_)
    {
        return;
    }
    // 矢印オブジェクトの作成
    if(!arrow_)
    {
        arrow_     = Scene::Object::Create<Object>("NaviArrow")->SetScaleAxisXYZ(float3{0.5f, 0.5f, 0.5f});
        auto model = arrow_->AddComponent<ComponentModel>("data/Game/Model/Arrow/Arrow.mv1");
        model->SetTranslate(float3{-7.5f, 55.0f, 0.0f})->SetRotationAxisXYZ(float3{0.0f, 180.0f, 0.0f})->UseForwardRendering(true);
        model->UseShader(false);
        model->RemoveZBuffer();
    }
    // プレイヤーの位置に追従
    arrow_->SetTranslate(player_->GetTranslate());
    // ボスエリアの方を向く
    if(enter_boss_checker)
    {
        if(!enter_boss_checker->IsHit())
        {
            arrow_->SetRotationToPosition(enter_boss_checker->GetTranslate());
        }
    }
}

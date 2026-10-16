//===========================================================================
//! @file Sound.cpp
//! @brief 音再生のコンポーネント
//===========================================================================
#include "Sound.h"
#include <Game/Pause.h>

//! @brief 初期化
void Sound::Init()
{
    Super::Init();
}

//! @brief 更新
void Sound::Update()
{
    Super::Update();
    // 再生中のサウンドの座標を更新
    for(auto& sound : sound_data_)
    {
        if(sound.second == -1)
        {
            continue;
        }

        if(CheckSoundMem(sound.second))
        {
            TYPE type = TYPE::SE;
            if(type_data_[sound.first] == TYPE::MUSIC)
            {
                type = TYPE::MUSIC;
            }
            float volume = SE_VOLUME;
            if(type == TYPE::MUSIC)
            {
                volume = MUSIC_VOLUME;
            }
            ChangeVolumeSoundMem(static_cast<int>(255.0f * volume * volume_data_[sound.first]), sound.second);
        }

        // 3Dサウンドでない場合はスキップ
        if(radius_data_[sound.first] <= 0.0f)
        {
            continue;
        }
        // 再生中であれば座標更新
        if(CheckSoundMem(sound.second))
        {
            Set3DPositionSoundMem(cast(GetOwner()->GetTranslate()), sound.second);
        }
    }
}

//! @brief 描画
void Sound::Draw()
{
    Super::Draw();

    if(!use_debug_)
    {
        return;
    }
    // 3Dサウンドの範囲を描画
    for(auto& sound : sound_data_)
    {
        if(sound.second == -1)
        {
            continue;
        }
        if(radius_data_[sound.first] <= 0.0f)
        {
            continue;
        }
        // 再生中であれば範囲を描画
        if(CheckSoundMem(sound.second))
        {
            SetUseLighting(false);
            DrawSphere3D(cast(GetOwner()->GetTranslate()), radius_data_[sound.first], 16, GetColor(255, 0, 0), GetColor(255, 0, 0), false);
            SetUseLighting(true);
        }
    }
}

//! @brief 終了
void Sound::Exit()
{
    Super::Exit();

    // データ解放
    for(int i = 0; i < handle_.size(); i++)
    {
        DeleteSoundMem(handle_[i]);
    }
    handle_.clear();
    play_types_.clear();
    paused_sounds_.clear();
    sound_data_.clear();
    radius_data_.clear();
    volume_data_.clear();
    type_data_.clear();
}

//! @brief GUI表示処理
void Sound::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("ComponentSound"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }

            ImGui::Checkbox(UNIQUE_TEXT(u8"デバッグ表示"), &use_debug_);

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//! @brief 音データを読み込む
//! @param sounds 読み込む音のリスト
void Sound::Load(const std::vector<Desc>& sounds)
{
    // 3Dサウンド用に設定
    SetCreate3DSoundFlag(true);

    // サウンドの読み込み
    for(auto& sound : sounds)
    {
        handle_.push_back(LoadSoundMem(sound.file_path_.c_str()));
        sound_data_[sound.name_]  = handle_.back();
        radius_data_[sound.name_] = sound.radius_;
        volume_data_[sound.name_] = sound.volume_;
        type_data_[sound.name_]   = sound.type_;
    }

    // 3Dサウンド用に解除
    SetCreate3DSoundFlag(false);
}

//! @brief 音データを読み込む
//! @param sounds 読み込む音のリスト
void Sound::Load2D(const std::vector<Desc>& sounds)
{
    // サウンドの読み込み
    for(auto& sound : sounds)
    {
        handle_.push_back(LoadSoundMem(sound.file_path_.c_str()));
        sound_data_[sound.name_]  = handle_.back();
        radius_data_[sound.name_] = -1.0f;
        volume_data_[sound.name_] = sound.volume_;
        type_data_[sound.name_]   = sound.type_;
    }
}

//! @brief 音を再生
//! @param name 音名
//! @param playtype 再生タイプ
void Sound::Play(std::string name, int playtype)
{
    if(IsPause())
    {
        return;
    }

    TYPE type = TYPE::SE;
    if(type_data_[name] == TYPE::MUSIC)
    {
        type = TYPE::MUSIC;
    }
    float volume = SE_VOLUME;
    if(type == TYPE::MUSIC)
    {
        volume = MUSIC_VOLUME;
    }
    // 音量を変更
    ChangeVolumeSoundMem(static_cast<int>(255.0f * volume * volume_data_[name]), sound_data_[name]);
    // 半径を変更
    Set3DRadiusSoundMem(radius_data_[name], sound_data_[name]);
    // 座標設定
    Set3DPositionSoundMem(cast(GetOwner()->GetTranslate()), sound_data_[name]);
    // 音を再生
    if(PlaySoundMem(sound_data_[name], playtype) == 0)
    {
        play_types_[sound_data_[name]] = playtype;
        paused_sounds_.erase(sound_data_[name]);
    }
    // 今再生しているハンドルを代入
    current_handle_ = sound_data_[name];
}

//! @brief 音を再生
//! @param name 音名
//! @param playtype 再生タイプ
void Sound::Play2D(std::string name, int playtype)
{
    if(IsPause())
    {
        return;
    }

    TYPE type = TYPE::SE;
    if(type_data_[name] == TYPE::MUSIC)
    {
        type = TYPE::MUSIC;
    }
    float volume = SE_VOLUME;
    if(type == TYPE::MUSIC)
    {
        volume = MUSIC_VOLUME;
    }
    // 音量を変更
    ChangeVolumeSoundMem(static_cast<int>(255.0f * volume * volume_data_[name]), sound_data_[name]);
    // 音を再生
    if(PlaySoundMem(sound_data_[name], playtype) == 0)
    {
        play_types_[sound_data_[name]] = playtype;
        paused_sounds_.erase(sound_data_[name]);
    }
    // 今再生しているハンドルを代入
    current_handle_ = sound_data_[name];
}

//! @brief 音を再生
//! @brief 音が再生している場合は再生しない
//! @param name 再生するサウンド名
//! @param playtype 再生タイプ
void Sound::PlayNoSame(std::string name, int playtype)
{
    // 音が再生していない場合
    if(!paused_sounds_.count(sound_data_[name]) && CheckSoundMem(sound_data_[name]) == 0)
    {
        // 音を再生
        Play(name, playtype);
    }
}

//! @brief 音を再生
//! @brief 音が再生している場合は再生しない
//! @param name 再生するサウンド名
//! @param playtype 再生タイプ
void Sound::PlayNoSame2D(std::string name, int playtype)
{
    // 音が再生していない場合
    if(!paused_sounds_.count(sound_data_[name]) && CheckSoundMem(sound_data_[name]) == 0)
    {
        // 音を再生
        Play2D(name, playtype);
    }
}

//! @brief 音を再生
//! @brief 同じサウンドなら再生しない
//! @param name 再生するサウンド名
//! @param playtype 再生タイプ
void Sound::PlayOnce(std::string name, int playtype)
{
    //  同じサウンドではない場合
    if(current_handle_ != sound_data_[name])
    {
        // 音を再生
        Play(name, playtype);
    }
}

//! @brief 音を再生
//! @brief 同じサウンドなら再生しない
//! @param name 再生するサウンド名
//! @param playtype 再生タイプ
void Sound::PlayOnce2D(std::string name, int playtype)
{
    //  同じサウンドではない場合
    if(current_handle_ != sound_data_[name])
    {
        // 音を再生
        Play2D(name, playtype);
    }
}

//! @brief 音再生を止める
//! @param name 音名
void Sound::Stop(std::string name)
{
    auto sound = sound_data_.find(name);
    if(sound == sound_data_.end())
    {
        return;
    }
    const int handle = sound->second;

    paused_sounds_.erase(handle);
    if(CheckSoundMem(handle) == 1)
    {
        StopSoundMem(handle);
    }
    if(current_handle_ == handle)
    {
        current_handle_ = -1;
    }
}

//! @brief 再生中か
//! @retval true 再生中
//! @retval false 再生ではない
bool Sound::IsPlaying()
{
    return paused_sounds_.count(current_handle_) != 0 || CheckSoundMem(current_handle_) == 1;
}

//! @brief サウンドの再生速度を設定
//! @param speed 設定する速度倍率
void Sound::SetSpeed(int speed)
{
    SetFrequencySoundMem(speed, current_handle_);
}

//! @brief ポーズに合わせて音を一時停止と再開
void Sound::UpdateMenuPause()
{
    auto* scene = Scene::GetCurrentScene();
    if(!scene)
    {
        return;
    }
    for(auto& object : scene->GetObjectPtrVec())
    {
        for(auto& sound : object->GetComponents<Sound>())
        {
            sound->SetMenuPaused(IsPause());
        }
    }
}

//! @brief メニューで一時停止するか
//! @param paused 一時停止するかどうか
void Sound::SetMenuPaused(bool paused)
{
    for(auto& sound : sound_data_)
    {
        int handle = sound.second;
        if(handle < 0)
        {
            continue;
        }
        if(paused)
        {
            if(!paused_sounds_.count(handle) && CheckSoundMem(handle) == 1)
            {
                auto type = play_types_.find(handle);
                if(type != play_types_.end() && StopSoundMem(handle) == 0)
                {
                    paused_sounds_[handle] = type->second;
                }
            }
        }
        else
        {
            auto stopped = paused_sounds_.find(handle);
            if(stopped == paused_sounds_.end())
            {
                continue;
            }
            float volume = SE_VOLUME;
            if(type_data_[sound.first] == TYPE::MUSIC)
            {
                volume = MUSIC_VOLUME;
            }
            ChangeVolumeSoundMem(static_cast<int>(255.0f * volume * volume_data_[sound.first]), handle);
            int type = DX_PLAYTYPE_BACK;
            if(stopped->second == DX_PLAYTYPE_LOOP)
            {
                type = DX_PLAYTYPE_LOOP;
            }
            PlaySoundMem(handle, DX_PLAYTYPE_BACK, FALSE);
            paused_sounds_.erase(stopped);
        }
    }
}

//===========================================================================
//! @file DialogueVoice.cpp
//! @brief 会話音声の再生管理
//===========================================================================
#include "DialogueVoice.h"
#include <Game/Pause.h>

//! @brief 更新
void DialogueVoice::Update()
{
    Super::Update();
    // ポーズ中は再生を開始しない
    if(IsPause() || !started_ || next_voice_ >= queue_.size())
    {
        return;
    }
    // 再生中の音声が終了したら次の音声を再生する
    if(next_voice_ == 0 || !Sound::IsPlaying())
    {
        Play2D(queue_[next_voice_++]);
    }
}

//! @brief 終了
void DialogueVoice::Exit()
{
    Super::Exit();

    Stop();

    load_voice_.clear();
    sequence_.clear();
}

//! @brief GUI表示処理
void DialogueVoice::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("DialogueVoice"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//! @brief 再生する会話音声を設定
//! @param sequence 番号除きの音声フォルダー名
//! @param message_index 再生する段落
//! @param message_count 連続再生する音声の数
void DialogueVoice::SetSequence(std::string_view sequence, int message_index, int message_count)
{
    std::string sequence_name(sequence);
    // 既存の音声を停止
    Stop();
    sequence_ = sequence_name;
    if(sequence_.empty())
    {
        return;
    }
    // 音声の読み込み
    for(int i = message_index; i < message_index + message_count; i++)
    {
        std::string number = std::to_string(i + 1);
        std::string name   = sequence_ + "_" + number;
        std::string path   = "data/Game/SE/DialogueVoice/" + sequence_ + "/" + name + ".mp3";

        Load2D({
            {name, path, 0.0f, 1.0f, Sound::TYPE::SE}
        });

        load_voice_.push_back(name);

        queue_.push_back(name);
    }
}

//! @brief 段落の音声に切り替えて再生する
//! @param message_index 再生する段落
void DialogueVoice::PlayMessage(int message_index)
{
    SetSequence(sequence_, message_index);
    Play();
}

//! @brief 会話音声を再生する
void DialogueVoice::Play()
{
    if(started_ || queue_.empty())
    {
        return;
    }
    started_ = true;
    if(!IsPause())
    {
        Play2D(queue_[next_voice_++]);
    }
}

//! @brief 会話音声の再生完了か
//! @retval true 再生中
//! @retval false 再生完了
bool DialogueVoice::IsPlaying()
{
    if(queue_.empty())
    {
        return false;
    }
    return !started_ || Sound::IsPlaying();
}

//! @brief 会話音声を停止
void DialogueVoice::Stop()
{
    // 再生中の音声を停止する
    for(const auto& name : load_voice_)
    {
        Sound::Stop(name);
    }
    queue_.clear();
    next_voice_ = 0;
    started_    = false;
}

#pragma once
//===========================================================================
//! @file DialogueVoice.h
//! @brief 会話音声の再生管理
//===========================================================================
#include <Game/Component/Sound.h>

USING_PTR(DialogueVoice);

//---------------------------------------------------------------------------
//! @class DialogueVoice
//! @brief 会話音声の再生管理コンポーネント
//---------------------------------------------------------------------------
class DialogueVoice : public Sound
{
public:
    BP_COMPONENT_DECL(DialogueVoice, u8"会話音声");

    DialogueVoice()
    {
        SetStatus(Component::StatusBit::SameType, true);
    }

    //! @brief 更新
    void Update() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 再生する会話音声を設定
    //! @param sequence 番号除きの音声フォルダー名
    //! @param message_index 再生する段落
    //! @param message_count 連続再生する音声の数
    void SetSequence(std::string_view sequence, int message_index = 0, int message_count = 1);

    //! @brief 段落の音声に切り替えて再生する
    //! @param message_index 再生する段落
    void PlayMessage(int message_index);

    //! @brief 会話音声を再生する
    void Play();

    //! @brief 会話音声を停止
    void Stop();

    //! @brief 会話音声の再生完了か
    //! @retval true 再生中
    //! @retval false 再生完了
    bool IsPlaying();

private:
    std::string sequence_;                  //!< 番号除きの音声フォルダー名
    std::vector<std::string> load_voice_;   //!< 音声名
    std::vector<std::string> queue_;        //!< 再生する順番
    int next_voice_ = 0;                    //!< 次に再生する音声
    bool started_   = false;                //!< 再生開始か
};

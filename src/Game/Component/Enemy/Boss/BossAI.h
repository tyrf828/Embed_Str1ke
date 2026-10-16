#pragma once
//===========================================================================
//! @file BossAI.h
//! @brief ボスのAIコンポーネント
//===========================================================================
#include <onnxruntime_cxx_api.h>
#include <array>

//---------------------------------------------------------------------------
//! @class BossAction
//! @brief ディープラーニングAI
//---------------------------------------------------------------------------
class BossAI : public Component
{
public:
    BP_COMPONENT_DECL(BossAI, u8"ディープラーニングAI");

    //! @brief 初期化
    void Init() override;

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 入力データ構造体
    struct Input
    {
        float distance;                       //!< プレイヤーとの距離
        int boss_hp;                          //!< ボスのHP
        int boss_max_hp;                      //!< ボスの最大HP
        int player_hp;                        //!< プレイヤーのHP
        int player_max_hp;                    //!< プレイヤーの最大HP
        int last_attack;                      //!< 前回の攻撃タイプ
        int hit_count;                        //!< 前回の攻撃開始以降の被弾回数
        std::array<float, 5> success_bonus;   //!< 攻撃成功ボーナスと失敗ペナルティ
    };

    //! @brief 入力データをまとめて、onnxの入力に必要な形式に変換
    //! @param input 入力データ構造体
    //! @param input_data 変換後の入力データ
    void CreateInputToData(Input& input, std::vector<float>& input_data);

    //! @brief 推論処理
    //! @param input_data 入力データ
    //! @param output_data 出力データ
    void Reasoning(std::vector<float>& input_data, std::vector<float>& output_data);

private:
    std::wstring model_path_ = L"data/Game/AI/boss_model.onnx";   //!< ONNXモデルのパス
    Ort::Env env_{ORT_LOGGING_LEVEL_FATAL, "BossAI"};             //!< ONNX Runtimeの環境
    Ort::SessionOptions session_options_;                         //!< セッションオプション
    std::shared_ptr<Ort::Session> session_;                       //!< セッション
    static constexpr int ACTION_SIZE_ = 5;                        //!< 攻撃の種類数
    std::vector<float> gui_output_;                               //!< GUIで表示するための変数
};

//===========================================================================
//! @file BossAI.cpp
//! @brief ボスのAIコンポーネント
//===========================================================================
#include "BossAI.h"

//! @brief 初期化
void BossAI::Init()
{
    Super::Init();
    // CPUのスレッド数を1
    session_options_.SetIntraOpNumThreads(1);
    // セクションの作成
    session_ = std::make_shared<Ort::Session>(env_, model_path_.data(), session_options_);

    gui_output_.resize(ACTION_SIZE_);
}

//! @brief 終了
void BossAI::Exit()
{
    Super::Exit();

    session_.reset();
}

//! @brief GUI表示処理
void BossAI::GUI()
{
    Super::GUI();

    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("BossAI"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::TreePop();
            ImGui::Separator();
            // 今回推論した結果を表示する
            constexpr std::array<const char*, ACTION_SIZE_> ACTION_NAMES = {u8"Kick", u8"Grab", u8"Dash", u8"FallbackDash", u8"Blast"};
            for(int i = 0; i < ACTION_SIZE_; i++)
            {
                ImGui::Text("%s : %f", ACTION_NAMES[i], gui_output_[i]);
            }
            ImGui::Separator();
            float max_score = *std::max_element(gui_output_.begin(), gui_output_.end());
            float sum       = 0.0f;
            std::array<float, ACTION_SIZE_> probability;
            for(int i = 0; i < ACTION_SIZE_; i++)
            {
                // Softmaxで確率に変換
                probability[i]  = std::exp(gui_output_[i] - max_score);
                sum            += probability[i];
            }
            for(int i = 0; i < ACTION_SIZE_; i++)
            {
                probability[i] /= sum;
            }

            std::string action_name  = ACTION_NAMES[0];
            float action_score       = gui_output_[0];
            float action_probability = probability[0];
            for(int i = 1; i < ACTION_SIZE_; i++)
            {
                if(gui_output_[i] > action_score)
                {
                    action_name        = ACTION_NAMES[i];
                    action_score       = gui_output_[i];
                    action_probability = probability[i];
                }
            }
            ImGui::Text("Model Chosen: %s (%.1f%%)", action_name.c_str(), action_probability * 100.0f);

            for(int i = 0; i < ACTION_SIZE_; i++)
            {
                ImGui::Text("%s %.1f%%", ACTION_NAMES[i], probability[i] * 100.0f);
            }

            ImGui::Separator();
        }
    }
    ImGui::End();
}

//! @brief 入力データをまとめて、onnxの入力に必要な形式に変換
//! @param input 入力データ構造体
//! @param input_data 変換後の入力データ
void BossAI::CreateInputToData(Input& input, std::vector<float>& input_data)
{
    const int INPUT_SIZE = 15;
    input_data.resize(INPUT_SIZE);
    // 距離(0~350の範囲)
    input_data[0] = std::clamp(input.distance, 0.0f, 350.0f) / 350.0f;
    // HP割合(0~1の範囲)
    input_data[1] = std::clamp(static_cast<float>(input.boss_hp) / input.boss_max_hp, 0.0f, 1.0f);
    input_data[2] = std::clamp(static_cast<float>(input.player_hp) / input.player_max_hp, 0.0f, 1.0f);
    // 前回の攻撃タイプ
    input_data[3] = float(input.last_attack == 0);    // 前回Kick
    input_data[4] = float(input.last_attack == 1);    // 前回Grab
    input_data[5] = float(input.last_attack == 2);    // 前回Dash
    input_data[6] = float(input.last_attack == 3);    // 前回FallbackDash
    input_data[7] = float(input.last_attack == 4);    // 前回Blast
    input_data[8] = float(input.last_attack == -1);   // 前回なし
    // 攻撃成功ボーナスと失敗ペナルティ(0.8~1.2の範囲)
    input_data[9]  = std::clamp(input.success_bonus[0], 0.8f, 1.2f);   // Kick補正
    input_data[10] = std::clamp(input.success_bonus[1], 0.8f, 1.2f);   // Grab補正
    input_data[11] = std::clamp(input.success_bonus[2], 0.8f, 1.2f);   // Dash補正
    input_data[12] = std::clamp(input.success_bonus[3], 0.8f, 1.2f);   // FallbackDash補正
    input_data[13] = std::clamp(input.success_bonus[4], 0.8f, 1.2f);   // Blast補正
    // 前回の攻撃以降の被弾回数
    input_data[14] = (float)std::clamp(input.hit_count, 0, 10) / 10.0f;
}

//! @brief 推論処理
//! @param input_data 入力データ
//! @param output_data 出力データ
void BossAI::Reasoning(std::vector<float>& input_data, std::vector<float>& output_data)
{
    output_data.resize(5);
    // 入出力のデータのサイズを確保
    // PyTorchの例です
    //torch::nn::Sequential model(
    //    torch::nn::Linear(15, 32),
    //    torch::nn::ReLU(),
    //    torch::nn::Linear(32, 5));
    std::vector<int64_t> input_shapes  = {1, 15};
    std::vector<int64_t> output_shapes = {1, 5};

    // メモリ情報を作成
    Ort::MemoryInfo memory_info(Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU));

    // 入出力のテンソルを作成
    // PyTorchの例です
    // torch::Tensor input_tensor = torch::from_blob(input_data.data(), {1, 15});
    Ort::Value input_tensor  = Ort::Value::CreateTensor<float>(memory_info, input_data.data(), input_data.size(), input_shapes.data(), input_shapes.size());
    Ort::Value output_tensor = Ort::Value::CreateTensor<float>(memory_info, output_data.data(), output_data.size(), output_shapes.data(), output_shapes.size());

    // 入出力のテンソルの名前
    // ここはONNXモデルの入力と出力の名前を取得する必要があります
    Ort::AllocatorWithDefaultOptions allocator;
    auto input_name            = session_->GetInputNameAllocated(0, allocator);
    auto output_name           = session_->GetOutputNameAllocated(0, allocator);
    const char* input_names[]  = {input_name.get()};
    const char* output_names[] = {output_name.get()};

    Ort::RunOptions run_options;
    // 推論を実行
    session_->Run(run_options, input_names, &input_tensor, 1, output_names, &output_tensor, 1);

    // 推論結果をGUIで表示
    gui_output_.clear();
    gui_output_ = output_data;
}

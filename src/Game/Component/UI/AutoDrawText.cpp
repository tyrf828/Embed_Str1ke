//===========================================================================
//! @file AutoDrawText.cpp
//! @brief 自動描画テキスト
//===========================================================================
#include "AutoDrawText.h"
#include <System/ProcTiming.h>
#include <System/Lib.h>

//! @brief 初期化
void AutoDrawText::Init()
{
    Super::Init();

    auto texts = GetOwner()->GetComponents<AutoDrawText>();
    int max    = -1;

    // ID取得
    for(AutoDrawTextPtr text : texts)
    {
        if((int)text->name_id_ > max)
        {
            max = (int)text->name_id_;
        }
    }

    name_id_     = max + 1;
    font_handle_ = CreateFontToHandle(NULL, (int)(font_size_ * SCALE_WINDOW), 2, DX_FONTTYPE_ANTIALIASING);

    SetProc("AutoDrawText:LateDraw", [this]() { LateDraw(); }, ProcTiming::LateDraw, ProcPriority::LOWEST);
}

//! @brief 更新
void AutoDrawText::Update()
{
    Super::Update();
    if(!is_draw_)
    {
        return;
    }
    // 描画速度が0以下の場合は全表示
    if(draw_speed_ <= 0.0f)
    {
        current_char_index_ = text_.size();
        return;
    }
    elapsed_time_ += GetDeltaTime();

    // 文字送り
    if(elapsed_time_ >= draw_speed_ && current_char_index_ < text_.size())
    {
        elapsed_time_ = 0.0f;

        int char_bytes       = GetCharBytes(DX_CHARCODEFORMAT_SHIFTJIS, &text_[current_char_index_]);
        current_char_index_ += char_bytes;
    }
}

//! @brief 遅延描画
void AutoDrawText::LateDraw()
{
    Super::LateDraw();
    if(is_debug_)
    {
        DrawBoxAA(box_size_top_left_.x, box_size_top_left_.y, box_size_bottom_right_.x, box_size_bottom_right_.y, GetColor(0, 255, 0), FALSE);
    }
    if(!is_draw_)
    {
        return;
    }

    if(current_char_index_ == 0)
    {
        return;
    }
    std::string display_text = text_.substr(0, current_char_index_);

    // ボックスサイズ内で改行しながら描画
    float current_x = box_size_top_left_.x;
    float current_y = box_size_top_left_.y;
    int line_height = GetFontSizeToHandle(font_handle_);

    if(alignment_ == TextAlignment::CENTER)
    {
        // 中央揃えの場合は行ごとに描画
        std::vector<std::string> lines;
        std::string current_line;

        int index = 0;
        while(index < display_text.size())
        {
            if(display_text[index] == '\n')
            {
                lines.push_back(current_line);
                current_line.clear();
                index++;
                continue;
            }

            int char_bytes       = GetCharBytes(DX_CHARCODEFORMAT_SHIFTJIS, &display_text[index]);
            std::string one_char = display_text.substr(index, char_bytes);

            current_line += one_char;
            index        += char_bytes;
        }

        if(!current_line.empty())
        {
            lines.push_back(current_line);
        }

        // 各行を中央揃えで描画
        for(const auto& line : lines)
        {
            if(current_y + line_height > box_size_bottom_right_.y)
            {
                break;
            }

            float center_x = (box_size_top_left_.x + box_size_bottom_right_.x) / 2.0f;
            DrawStringMiddleToHandle(center_x, current_y + line_height * 0.5f, line.c_str(), color_, font_handle_);
            current_y += line_height;
        }
    }
    else
    {
        // 左揃えの場合
        int index = 0;
        while(index < display_text.size())
        {
            if(display_text[index] == '\n')
            {
                current_x  = box_size_top_left_.x;
                current_y += line_height;
                index++;

                if(current_y + line_height > box_size_bottom_right_.y)
                {
                    break;
                }
                continue;
            }

            // 1文字のバイト数を取得
            int char_bytes = GetCharBytes(DX_CHARCODEFORMAT_SHIFTJIS, &display_text[index]);

            // 1文字分の文字列を取得
            std::string one_char = display_text.substr(index, char_bytes);

            // 1文字の幅を取得
            int char_width = GetDrawStringWidthToHandle(one_char.c_str(), (int)one_char.size(), font_handle_);

            // ボックスの右端を超える場合は改行
            if(current_x + char_width > box_size_bottom_right_.x && current_x > box_size_top_left_.x)
            {
                current_x  = box_size_top_left_.x;
                current_y += line_height;

                // ボックスの下端を超える場合は描画を中断
                if(current_y + line_height > box_size_bottom_right_.y)
                {
                    break;
                }
            }

            // 文字を描画
            DrawStringFToHandle(current_x, current_y, one_char.c_str(), color_, font_handle_);

            // 次の文字の位置へ
            current_x += char_width;
            index     += char_bytes;
        }
    }
}

//! @brief 終了
void AutoDrawText::Exit()
{
    Super::Exit();
}

//! @brief GUI表示処理
void AutoDrawText::GUI()
{
    assert(GetOwner());
    auto obj_name = GetOwner()->GetName();

    ImGui::Begin(obj_name.data());
    {
        ImGui::Separator();

        auto ui_name = std::string("AutoDrawText ") + std::to_string(name_id_);

        if(ImGui::TreeNode(ui_name.c_str()))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::DragFloat2(UNIQUE_TEXT("Box Top Left"), (float*)&box_size_top_left_, 0.1f);
            ImGui::DragFloat2(UNIQUE_TEXT("Box Bottom Right"), (float*)&box_size_bottom_right_, 0.1f);

            // WINDOW_W と WINDOW_H に対する比率を計算して表示
            float ratio_top_left_x     = box_size_top_left_.x / WINDOW_W;
            float ratio_top_left_y     = box_size_top_left_.y / WINDOW_H;
            float ratio_bottom_right_x = box_size_bottom_right_.x / WINDOW_W;
            float ratio_bottom_right_y = box_size_bottom_right_.y / WINDOW_H;
            ImGui::Text("Box Top Left In Window: %.2f , %.2f ", ratio_top_left_x, ratio_top_left_y);
            ImGui::Text("Box Bottom Right In Window: %.2f , %.2f ", ratio_bottom_right_x, ratio_bottom_right_y);
            ImGui::Checkbox(UNIQUE_TEXT("Draw Debug Box"), &is_debug_);
            if(ImGui::Button("Start Draw"))
            {
                StartDraw();
            }
            ImGui::Separator();

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//! @brief テキストを設定
//! @param text テキスト
AutoDrawTextPtr AutoDrawText::SetText(std::string_view text)
{
    text_ = text;
    return std::dynamic_pointer_cast<AutoDrawText>(shared_from_this());
}

//! @brief テキストデータを設定
//! @param data_path データパス
AutoDrawTextPtr AutoDrawText::SetTextData(std::string_view data_path)
{
    int file_handle = FileRead_open(data_path.data());
    if(file_handle == 0)
    {
        return std::dynamic_pointer_cast<AutoDrawText>(shared_from_this());
    }

    auto file_size = FileRead_size(data_path.data());
    std::string buffer;
    buffer.resize(file_size);

    FileRead_read(buffer.data(), (int)file_size, file_handle);
    FileRead_close(file_handle);

    text_ = buffer;
    return std::dynamic_pointer_cast<AutoDrawText>(shared_from_this());
}

//! @brief 描画ボックスを設定
//! @param box_size_top_left 左上座標
//! @param box_size_bottom_right 右下座標
AutoDrawTextPtr AutoDrawText::SetDrawBox(float2 box_size_top_left, float2 box_size_bottom_right)
{
    box_size_top_left_     = box_size_top_left;
    box_size_bottom_right_ = box_size_bottom_right;
    return std::dynamic_pointer_cast<AutoDrawText>(shared_from_this());
}

//! @brief 色を設定
//! @param color 色
AutoDrawTextPtr AutoDrawText::SetColor(int color)
{
    color_ = color;
    return std::dynamic_pointer_cast<AutoDrawText>(shared_from_this());
}

//! @brief テキストファイルを読み込む
//! @param file_path ファイルパス
//! @return テキストの行ごとのベクター
std::vector<std::string> AutoDrawText::LoadTextFile(std::string_view file_path)
{
    std::vector<std::string> lines;

    int file_handle = FileRead_open(file_path.data());
    if(file_handle == 0)
    {
        return lines;
    }

    std::string current_line;
    char buffer[256];

    while(FileRead_gets(buffer, sizeof(buffer), file_handle) != -1)
    {
        std::string str_line(buffer);

        if(!current_line.empty())
        {
            lines.push_back(current_line);
            current_line.clear();
        }

        current_line += str_line;
    }

    if(!current_line.empty())
    {
        lines.push_back(current_line);
    }

    FileRead_close(file_handle);
    return lines;
}

//===========================================================================
//! @file AddData.cpp
//! @brief 座標追加ツール
//===========================================================================
#include "AddData.h"
#include <System/Debug/DebugCamera.h>
#include <fstream>
#include <string>

//! @brief 初期化
//! @return 初期化済み
bool AddData::Init()
{
    Super::Init();
    SetName(u8"座標追加ツール");
    return true;
}

//! @brief GUI表示
void AddData::GUI()
{
    Super::GUI();

    ImGui::Begin(u8"座標追加ツール");
    {
        // ファイルパス入力
        char path_buffer[256];
        strcpy_s(path_buffer, file_path_.c_str());
        if(ImGui::InputText(u8"ファイルパス", path_buffer, 256))
        {
            file_path_ = path_buffer;
        }

        // デバッグカメラの座標を表示
        if(DebugCamera::IsUse())
        {
            ImGui::Separator();
            auto cam = DebugCamera::GetCamera().lock();
            if(cam)
            {
                float3 pos = cam->GetPosition();
                ImGui::Text(u8"デバッグカメラ座標: %.2f, %.2f, %.2f", (float)pos.x, (float)pos.y, (float)pos.z);

                ImGui::Separator();

                // 座標を追加するボタン
                if(ImGui::Button(u8"座標を追加"))
                {
                    AddDataTransform();
                }
            }
        }
    }
    ImGui::End();
}

//! @brief 座標データを追加
void AddData::AddDataTransformWithPos(const float3& pos)
{
    std::ofstream file(file_path_, std::ios::app);
    if(!file.is_open())
    {
        return;
    }
    // 小数点以下2桁まで保存
    file << std::fixed << std::setprecision(2);
    file << pos.x << "," << pos.y << "," << pos.z << "\n";
    file.close();
}

//! @brief パスの中のデータをクリア
void AddData::ClearData()
{
    std::ofstream file(file_path_, std::ios::trunc);
    file.close();
}

//! @brief 座標データを追加
void AddData::AddDataTransform()
{
    auto cam   = DebugCamera::GetCamera().lock();
    float3 pos = cam->GetPosition();

    std::ofstream file(file_path_, std::ios::app);
    if(!file.is_open())
    {
        return;
    }
    // 小数点以下2桁まで保存
    file << std::fixed << std::setprecision(2);
    file << pos.x << "," << pos.y << "," << pos.z << "\n";
    file.close();
}

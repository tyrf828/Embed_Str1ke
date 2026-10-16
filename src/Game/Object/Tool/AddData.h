//===========================================================================
//! @file AddData.h
//! @brief 座標追加ツール
//===========================================================================
#pragma once

//---------------------------------------------------------------------------
//! @class AddData
//! @brief 座標追加ツール
//---------------------------------------------------------------------------
class AddData : public Object
{
public:
    BP_OBJECT_DECL(AddData, u8"座標追加ツール");

    //! @brief 初期化
    //! @return 初期化済み
    bool Init() override;

    //! @brief GUI表示
    void GUI() override;

    //! @brief パスを設定
    void SetPath(const std::string& path)
    {
        file_path_ = path;
    }

    //! @brief 座標データを追加
    void AddDataTransformWithPos(const float3& pos);

    //! @brief パスの中のデータをクリア
    void ClearData();

private:
    //! @brief 座標データを追加
    void AddDataTransform();

    std::string file_path_ = "data/Game/";   //!< データ保存パス
};

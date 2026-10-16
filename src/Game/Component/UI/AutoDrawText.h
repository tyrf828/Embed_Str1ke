#pragma once
//===========================================================================
//! @file AutoDrawText.h
//! @brief 自動描画テキスト
//===========================================================================

USING_PTR(AutoDrawText);

//---------------------------------------------------------------------------
//! @class AutoDrawText
//! @brief 自動描画テキスト
//---------------------------------------------------------------------------
class AutoDrawText : public Component
{
public:
    BP_COMPONENT_DECL(AutoDrawText, u8"自動描画テキスト");

    AutoDrawText()
    {
        // 同じタイプのコンポーネントを許可
        SetStatus(Component::StatusBit::SameType, true);
    }

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief 遅延描画
    void LateDraw();

    //! @brief 終了
    void Exit() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief テキストの配置
    enum struct TextAlignment
    {
        LEFT,    //!< 左揃え
        CENTER   //!< 中央揃え
    };

    //! @brief テキストを設定
    //! @param text テキスト
    AutoDrawTextPtr SetText(std::string_view text);

    //! @brief テキストデータを設定
    //! @param data_path データパス
    AutoDrawTextPtr SetTextData(std::string_view data_path);

    //! @brief 描画ボックスを設定
    //! @param box_size_top_left 左上座標
    //! @param box_size_bottom_right 右下座標
    AutoDrawTextPtr SetDrawBox(float2 box_size_top_left, float2 box_size_bottom_right);

    //! @brief 色を設定
    //! @param color 色
    AutoDrawTextPtr SetColor(int color);

    //! @brief 色を取得
    //! @return 色
    int GetTextColor() const
    {
        return color_;
    }

    //! @brief 設定されている文字列を取得
    std::string GetText() const
    {
        return text_;
    }

    //! @brief 描画に使用するフォントハンドルを取得
    int GetFontHandle() const
    {
        return font_handle_;
    }

    //! @brief フォントサイズを設定
    //! @param size フォントサイズ
    AutoDrawTextPtr SetFontSize(int size)
    {
        font_size_ = size;
        DeleteFontToHandle(font_handle_);
        font_handle_ = CreateFontToHandle(NULL, int(font_size_ * SCALE_WINDOW), 2, DX_FONTTYPE_ANTIALIASING);
        return std::dynamic_pointer_cast<AutoDrawText>(shared_from_this());
    }

    //! @brief 描画開始
    //! @param is_reset リセットするか
    void StartDraw(bool is_reset = true)
    {
        if(!is_reset)
        {
            is_draw_ = true;
            return;
        }
        is_draw_            = true;
        current_char_index_ = 0;
        elapsed_time_       = 0.0f;
    }

    //! @brief 描画速度を設定
    //! @param speed 描画速度
    AutoDrawTextPtr SetDrawSpeed(float speed)
    {
        draw_speed_ = speed;
        return std::dynamic_pointer_cast<AutoDrawText>(shared_from_this());
    }

    //! @brief 描画速度を取得
    //! @return 描画速度
    float GetDrawSpeed() const
    {
        return draw_speed_;
    }

    //! @brief 配置を設定
    //! @param alignment 配置
    AutoDrawTextPtr SetAlignment(TextAlignment alignment)
    {
        alignment_ = alignment;
        return std::dynamic_pointer_cast<AutoDrawText>(shared_from_this());
    }

    //! @brief 配置を取得
    //! @return 配置
    TextAlignment GetAlignment() const
    {
        return alignment_;
    }

    //! @brief 描画完了か
    //! @return 描画完了ならtrue
    bool IsDrawComplete() const
    {
        return current_char_index_ >= text_.size();
    }

    //! @brief テキストファイルを読み込む
    //! @param file_path ファイルパス
    //! @return テキストの行ごとのベクター
    std::vector<std::string> LoadTextFile(std::string_view file_path);

private:
    bool is_draw_                 = false;                     //!< 描画中か
    std::string text_             = "Test";                    //!< テキスト
    float2 box_size_top_left_     = {0.0f, 0.0f};              //!< ボックス左上座標
    float2 box_size_bottom_right_ = {0.0f, 0.0f};              //!< ボックス右下座標
    int color_                    = GetColor(255, 255, 255);   //!< 色
    int font_handle_              = -1;                        //!< フォントハンドル
    int font_size_                = DEFAULT_FONT_SIZE;         //!< フォントサイズ
    float elapsed_time_           = 0.0f;                      //!< 経過時間
    float draw_speed_             = 0.05f;                     //!< 何秒ごとに1文字表示する
    size_t current_char_index_    = 0;                         //!< 現在表示している文字のインデックス
    int name_id_                  = 0;                         //!< 名前ID
    bool is_debug_                = false;                     //!< デバッグ表示するか
    TextAlignment alignment_      = TextAlignment::LEFT;       //!< 配置
};

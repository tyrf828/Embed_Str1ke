#include "WinMain.h"
#include "Game/Pause.h"
#include <Game/Component/Sound.h>
#include <System/SystemMain.h>
#include <System/Utils/IniFileLib.h>

s32 WINDOW_W       = 1280;
s32 WINDOW_H       = 720;
bool IS_EXIT       = false;
bool IS_PAUSE      = false;
float MUSIC_VOLUME = 0.5f;
float SE_VOLUME    = 0.5f;
f32 SCALE_WINDOW   = 1.0f;

//---------------------------------------------------------------------------
//! アプリケーションエントリーポイント
//---------------------------------------------------------------------------
int WINAPI WinMain(_In_ [[maybe_unused]] HINSTANCE hInstance,
                   _In_opt_ [[maybe_unused]] HINSTANCE hPrevInstance,
                   _In_ [[maybe_unused]] LPSTR lpCmdLine,
                   _In_ [[maybe_unused]] int nShowCmd)
{
    // 高DPI対応
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    SetOutApplicationLogValidFlag(FALSE);

    // Game.iniから読み込む
    IniFileLib ini("Game.ini");
    bool is_fullscreen          = ini.GetBool("System", "FullScreen");
    float2 screen_size          = ini.GetFloat2("System", "ScreenSize", {WINDOW_W, WINDOW_H});
    const auto title_name       = ini.GetString("System", "Title", "Embed Str1ke");
    const bool use_keyboard     = ini.GetBool("System", "GUIUseKeyboard", false);
    const int use_device_number = ini.GetInt("System", "DeviceNumber", 0);

    // USE GPU
    DxLib::SetUseDirect3D11AdapterIndex(use_device_number);

    WINDOW_W = static_cast<int>(screen_size.x);
    WINDOW_H = static_cast<int>(screen_size.y);

    SetGraphMode(WINDOW_W, WINDOW_H, 32);

    float sx = (float)WINDOW_W / 1280.0f;
    float sy = (float)WINDOW_H / 720.0f;
    if(sx < sy)
    {
        SCALE_WINDOW = sx;
    }
    else
    {
        SCALE_WINDOW = sy;
    }

    // ウィンドウモード / フルスクリーンモード 切り替え
    ChangeWindowMode(!is_fullscreen);

    SetWindowIconID(101);

    if(!is_fullscreen)
    {
        // 高DPI対応のための自動リサイズを抑制するために手動でウィンドウサイズを指定。
        // 但し、フルスクリーンモードでこれを実行すると描画範囲が壊れるためウィンドウモード時のみ実行
        SetWindowSize(WINDOW_W, WINDOW_H);
    }

    SetBackgroundColor(0, 0, 0);
    SetMainWindowText(title_name.c_str());
    SetAlwaysRunFlag(true);   // ウィンドウメッセージを常に実行

    // DirectX11を使用するようにする(DxLib_Init関数前) - Effekseer対応
    SetUseDirect3DVersion(DX_DIRECT3D_11);

    // デプスバッファの精度向上
    SetZBufferBitDepth(32);

    // 非同期読み込み処理を行うスレッドの数を設定
    SetASyncLoadThreadNum(4);

    // フルサイズの時はウインドウ枠を出さない
    int width  = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    SetUseBorderlessWindowFlag(FALSE);
    if(width == screen_size.x && height == screen_size.y)
    {
        SetUseBorderlessWindowFlag(TRUE);
    }
#if defined(_DEBUG)
    //SetUseDirect3D11DebugDeviceFlag(TRUE);
#endif
    if(DxLib_Init() == -1)
    {
        return -1;
    }

    if(!is_fullscreen)   // GPU位置固定するため、初期化後行う必要がある
    {
        if(ini.GetBool("System", "GUIEditor"))
        {
            RECT rect{};
            float2 ofs = {0, 0};
            GetWindowClientRect(&rect);
            int screen_width  = GetSystemMetrics(SM_CXSCREEN);
            int screen_height = GetSystemMetrics(SM_CYSCREEN);
            if(rect.left > screen_width)
            {
                ofs.x = (float)screen_width;
            }
            if(rect.top > screen_height)
            {
                ofs.y = (float)screen_height;
            }
            int pos_x = std::max((screen_width - (WINDOW_W + 400)) / 2 + (int)ofs.x, 0);
            int pos_y = std::max((screen_height - (WINDOW_H + 200)) / 2 + (int)ofs.y, 0);
            SetWindowPosition(pos_x, pos_y);
        }
    }

    // Effekseerの初期化
    if(Effekseer_Init(8000) == -1)
    {
        DxLib_End();
        return -1;
    }

    // Effekseer対応
    SetChangeScreenModeGraphicsSystemResetFlag(FALSE);
    Effekseer_SetGraphicsDeviceLostCallbackFunctions();

    SetDrawScreen(DX_SCREEN_BACK);
    SetTransColor(255, 0, 255);
    srand(GetNowCount() % RAND_MAX);
    SRand(GetNowCount() % RAND_MAX);

    SetCameraNearFar(1.0f, 150.0f);
    SetupCamera_Perspective(D2R(45.0f));

    //SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);
    //----------------------------------------------------------
    // 初期化処理
    //----------------------------------------------------------
    InputKeyInit();
    InputPadInit();
    InputMouseInit();
    RenderInit();   // Render初期化
    SystemInit();
    PauseInit();
    ImGuiInit(use_keyboard);
    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);

    //----------------------------------------------------------
    // メインループ
    //----------------------------------------------------------
#if DEBUG
    while(ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0 && !IS_EXIT && !IsProcEnd())
#else
    while(ProcessMessage() == 0 && IS_EXIT == 0 && !IsProcEnd())
#endif
    {
        // 1フレームの開始
        SystemBeginFrame();

        // 描画先の設定と画面クリア
        SetRenderTarget(GetBackBuffer(), GetDepthStencil());
        ClearColor(GetBackBuffer(), float4(0.5, 0.5f, 0.5f, 0.0f));
        ClearDepth(GetDepthStencil(), 1.0f);

        clsDx();

        InputKeyUpdate();
        InputPadUpdate();
        InputMouseUpdate();
        ImGuiUpdate();

        Shader::updateFileWatcher();   // ファイル監視を更新

        // ---------------
        // 更新処理
        // ---------------
        PauseUpdate();
        SystemUpdate();
        Sound::UpdateMenuPause();
        Effekseer_Sync3DSetting();
        UpdateEffekseer3D();

        // ---------------
        // 描画処理
        // ---------------
        SystemDraw();
        DrawEffekseer3D();
        ImGuiDraw();
        PauseDraw();

        // 1フレームの終了
        SystemEndFrame();

        // ---------------
        // 画面更新
        // ---------------
        ScreenFlip();
    }

    //----------------------------------------------------------
    // 終了処理
    //----------------------------------------------------------
    ImGuiExit();
    InputKeyExit();
    InputPadExit();
    InputMouseExit();
    PauseExit();
    SystemExit();
    RenderExit();   // Render終了

    Effkseer_End();
    WaitHandleASyncLoadAll();   // 非同期ロード中のハンドルを全て待つ
    DxLib_End();

    return 0;
}

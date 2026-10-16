#pragma once

extern s32 WINDOW_W;
extern s32 WINDOW_H;
extern bool IS_EXIT;
extern bool IS_PAUSE;
extern float MUSIC_VOLUME;
extern float SE_VOLUME;
extern f32 SCALE_WINDOW;

// 1フレームのCPU処理時間を取得(単位:μsec)
u64 GetCpuProfile();
